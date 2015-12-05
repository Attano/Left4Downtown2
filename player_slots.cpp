/**
 * vim: set ts=4 :
 * =============================================================================
 * Left 4 Downtown SourceMod Extension
 * Copyright (C) 2009-2011 Downtown1, ProdigySim; 2012-2015 Visor
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *
 * Version: $Id$
 */

#include "extension.h"
#include "player_slots.h"
#include "asm/asm.h"
#include "CDetour/detourhelpers.h"

#define OP_JMP_REL8 '\xEB'
#define OP_JL_REL8  '\x7C'
#define OP_JLE_REL8 '\x7E'
#define OP_JLE_REL8_SIZE 2
#define OP_JZ_REL8  '\x74'
#define OP_JZ_REL8_SIZE 2

//this is actually a near return
#define OP_RETN     '\xC3'
#define OP_RETN_SIZE 1

//CMP EAX, imm32
#define OP_CMP_EAX_IMM32 '\x3D'
#define OP_CMP_EAX_IMM32_SIZE 5

#define OP_CMP_R32_RM32 '\x3B'
#define OP_CMP_R32_RM32_SIZE 6

//CMP r/m32, IMM32 ... byte opcode is 81/7
#define OP_CMP_RM32_IMM32 '\x81'
#define OP_CMP_RM32_IMM32_SIZE 6
#define OP_CMP_RM32_IMM32_MODRM_DIGIT ((char)(7 << 3))

//MOV EAX, imm32
#define OP_MOV_EAX_IMM32 '\xB8'
#define OP_MOV_EAX_IMM32_SIZE 5

#define MODRM_BYTE 1
#define MODRM_MOD_DIRECT ((char)(3 << 6))
#define MODRM_RM_EDX '\x02'
#define MODRM_RM_ESI '\x06'
#define MODRM_RM_EDI '\x07'

#if defined PLATFORM_WINDOWS
static void *humanLimitSig = NULL;
#endif
static void *lobbyConnectSig = NULL;
static int serverFullOffset = -1;

static patch_t humanLimitRestore;
static patch_t lobbyConnectRestore;
static patch_t serverFullRestore;

extern ConVar g_MaxPlayers;

int PlayerSlots::MaxSlots = -1;
int PlayerSlots::MaxClients = -1;
int PlayerSlots::MaxPlayers = -1;

#define PLAYER_SLOTS_MAX MaxSlots

void PlayerSlots::Patch()
{
#if defined PLATFORM_WINDOWS
	bool firstTime = !(humanLimitSig && lobbyConnectSig);
#else
	bool firstTime = !(lobbyConnectSig);
#endif
	L4D_DEBUG_LOG("PlayerSlots - Patching ...");

	/*
	Server patch
	** Needed on windows only. On Linux, this called GetMaxHumanPlayers
	L4DToolZ doesn't patch this anymore?
	*/
	
#if defined PLATFORM_WINDOWS
	if(firstTime)
	{
		if (!g_pGameConf->GetMemSig("HumanPlayerLimitReached", &humanLimitSig) || !humanLimitSig) 
		{
			g_pSM->LogError(myself, "PlayerSlots -- Could not find 'HumanPlayerLimitReached' signature");
			return;
		}
	}
	//code pages can't be written to by default, so ApplyPatch changes that ;)

	/*
	jl around the string "Human player limit reached (%d/%d)"

		jl -> jmp (Windows)
		jle -> nop (Linux)

	we never check if the human player limit has been reached
	*/
	patch_t humanLimitPatch;

//#if defined PLATFORM_WINDOWS
	humanLimitPatch.bytes = 1;
	humanLimitPatch.patch[0] = OP_JMP_REL8;
//#else //PLATFORM_LINUX
//	humanLimitPatch.bytes = OP_JLE_REL8_SIZE;
//	fill_nop(humanLimitPatch.patch, humanLimitPatch.bytes);
//#endif
	
	
	ApplyPatch(humanLimitSig, /*offset*/0, &humanLimitPatch, firstTime ? &humanLimitRestore : NULL);

	L4D_DEBUG_LOG("PlayerSlots -- 'HumanPlayerLimitReached' jl(e) patched to jmp"); 
#endif

	/*
	Engine patch
	*/
	if (!g_pGameConf->GetMemSig("ConnectClientLobbyCheck", &lobbyConnectSig) || !lobbyConnectSig) 
	{ 
		g_pSM->LogError(myself, "PlayerSlots -- Could not find 'ConnectClientLobbyCheck' signature");
		return;
	}
#if defined PLATFORM_WINDOWS

	/*
	jz around the string "#Valve_Reject_Server_Full"

		jz -> nop (linux or windows)

	 do not skip to the server is full code when sv_allow_lobby_connect is 0
	*/
	patch_t lobbyConnectPatch;
	lobbyConnectPatch.bytes = OP_JZ_REL8_SIZE;
	fill_nop(lobbyConnectPatch.patch, OP_JZ_REL8_SIZE);
	ApplyPatch(lobbyConnectSig, /*offset*/0, &lobbyConnectPatch, firstTime ? &lobbyConnectRestore : NULL);

	unsigned char oldValue2 = lobbyConnectRestore.patch[0];
	L4D_DEBUG_LOG("PlayerSlots -- 'ConnectClientLobbyCheck' jz(%x) patched to 2 nops", oldValue2);
#endif
}

static void *getMaxHumanPlayersSig = NULL;
static patch_t getMaxHumanPlayersRestore;

/*
	Patch CTerrorGameRules::GetMaxHumanPlayers(void) to always return our own value

	This will cause server browsers to display our own 'max players' value (Linux only)

	On Windows overriding it does nothing at first..
	  but actually it breaks scavenge mode which is stuck in a 'scavenge versus' type mode
	  so it should NEVER BE PATCHED FOR WINDOWS EVER

	On Linux just overriding ServerPlayerCounts isn't enough (it only fixes 'status' command)
	  but overwriting GetMaxHumanPlayers works great
*/
void PlayerSlots::PatchGetMaxHumanPlayers()
{
#if !defined PLATFORM_WINDOWS

	bool firstTime = (getMaxHumanPlayersSig == NULL);

	if(firstTime)
	{
		if (!g_pGameConf->GetMemSig("GetMaxHumanPlayers", &getMaxHumanPlayersSig) || !getMaxHumanPlayersSig) 
		{ 
			g_pSM->LogError(myself, "PlayerSlots -- Could not find 'GetMaxHumanPlayers' signature");
			return;
		} 
	}

	/*
	Normally CTerrorGameRules::GetMaxHumanRules(void)
		returns CTerrorGameRules::IsVersusMode() ? 8 : 4
	we just want it to return our own PLAYER_SLOTS_MAX always though
	*/

	/*
	The trick here is just to patch the function to:

		mov eax, PLAYER_SLOTS_MAX
		retn

	Now it will always return our value instead of 4 or 8
	*/

	patch_t getMaxHumanPlayersPatch;
	getMaxHumanPlayersPatch.bytes = OP_MOV_EAX_IMM32_SIZE + OP_RETN_SIZE;

	// mov eax, PLAYER_SLOTS_MAX
	getMaxHumanPlayersPatch.patch[0] = OP_MOV_EAX_IMM32;
	*(uint32_t*)(getMaxHumanPlayersPatch.patch+sizeof(uint8_t)) = (uint32_t)PLAYER_SLOTS_MAX;

	// retn
	getMaxHumanPlayersPatch.patch[OP_MOV_EAX_IMM32_SIZE] = OP_RETN;

	if(firstTime)
	{
		ApplyPatch(getMaxHumanPlayersSig, /*offset*/0, &getMaxHumanPlayersPatch, &getMaxHumanPlayersRestore);
	}
	else
	{
		ApplyPatch(getMaxHumanPlayersSig, /*offset*/0, &getMaxHumanPlayersPatch, /*restore*/NULL);
	}

	L4D_DEBUG_LOG("PlayerSlots -- 'GetMaxHumanPlayers' patched to MOV eax, %d; retn", PLAYER_SLOTS_MAX);

#endif
}

/*
static void *getTotalNumPlayersSupportedSig = NULL;
static patch_t getTotalNumPlayersSupportedRestore;

void PlayerSlots::PatchGetTotalNumPlayersSupported()
{
        bool firstTime = (getTotalNumPlayersSupportedSig == NULL);

        if(firstTime)
        {
                if (!g_pGameConf->GetMemSig("GetTotalNumPlayersSupported", &getTotalNumPlayersSupportedSig) || !getTotalNumPlayersSupportedSig)
                {
                        g_pSM->LogError(myself, "PlayerSlots -- Could not find 'GetTotalNumPlayersSupported' signature");
                        return;
                }
        }

        patch_t getTotalNumPlayersSupportedPatch;
        getTotalNumPlayersSupportedPatch.bytes = OP_MOV_EAX_IMM32_SIZE + OP_RETN_SIZE;

        // mov eax, PLAYER_SLOTS_MAX
        getTotalNumPlayersSupportedPatch.patch[0] = OP_MOV_EAX_IMM32;
        *(uint32_t*)(getTotalNumPlayersSupportedPatch.patch+sizeof(uint8_t)) = (uint32_t)PLAYER_SLOTS_MAX;

        // retn
        getTotalNumPlayersSupportedPatch.patch[OP_MOV_EAX_IMM32_SIZE] = OP_RETN;

        if(firstTime)
        {
                ApplyPatch(getTotalNumPlayersSupportedSig, 0, &getTotalNumPlayersSupportedPatch, &getTotalNumPlayersSupportedRestore);
        }
        else
        {
                ApplyPatch(getTotalNumPlayersSupportedSig, 0, &getTotalNumPlayersSupportedPatch, NULL);
        }

        L4D_DEBUG_LOG("PlayerSlots -- 'GetTotalNumberOfPlayersSupported' patched to MOV eax, %d; retn", PLAYER_SLOTS_MAX);
}

* EOP */

void PlayerSlots::PatchSlotCheckOnly()
{
	if(!lobbyConnectSig) return; //avoid a null dereference if we failed to lookup the sig

	bool firstTime = (serverFullOffset == -1);

	if(firstTime)
	{
		//////////////////////////
		//valve reject server full

		if (!g_pGameConf->GetOffset("ValveRejectServerFullFirst", &serverFullOffset))
		{
			g_pSM->LogError(myself, "PlayerSlots -- Could not find 'ValveRejectServerFullFirst' offset");
			return;
		}

		//the offset should point to the cmp eax, [esi+180h]... first byte incorrect => wrong offset

		if(*((uint8_t*)lobbyConnectSig + serverFullOffset) != OP_CMP_R32_RM32)
		{
			g_pSM->LogError(myself, "PlayerSlots -- Offset for 'ValveRejectServerFullFirst' is incorrect");
			return;
		}
	}

	/*
	cmp around the string "#Valve_Reject_Server_Full"

	 cmp eax, [esi+180h] -> cmp eax, IMM32(PLAYER_SLOTS_MAX)        (Windows)
	 cmp esi, [ebx+17Ch] -> cmp esi, IMM32(PLAYER_SLOTS_MAX)        (Linux)

	we effectively change how many max players we allow
	*/

	patch_t serverFullPatch;

#if defined PLATFORM_LINUX
	serverFullPatch.bytes  = OP_CMP_RM32_IMM32_SIZE;
	serverFullPatch.patch[0] = OP_CMP_RM32_IMM32;
	serverFullPatch.patch[MODRM_BYTE] = MODRM_MOD_DIRECT | OP_CMP_RM32_IMM32_MODRM_DIGIT | MODRM_RM_ESI; //0xFF
	*(uint32_t*)(serverFullPatch.patch+MODRM_BYTE+sizeof(uint8_t)) = (uint32_t)PLAYER_SLOTS_MAX;

#else //PLATFORM_WINDOWS
	serverFullPatch.bytes = OP_CMP_R32_RM32_SIZE;

	//replace the first part of cmp eax, [esi+180h] with cmp eax, IMM32(player_slots_max)

	serverFullPatch.patch[0] = OP_CMP_EAX_IMM32;
	*(uint32_t*)(serverFullPatch.patch+sizeof(uint8_t)) = (uint32_t)PLAYER_SLOTS_MAX;

	//fill in the rest of the patch size with nops

	fill_nop(serverFullPatch.patch + OP_CMP_EAX_IMM32_SIZE, OP_CMP_R32_RM32_SIZE - OP_CMP_EAX_IMM32_SIZE);
#endif

	if(firstTime)
	{
		ApplyPatch(lobbyConnectSig, serverFullOffset, &serverFullPatch, &serverFullRestore);
	}
	else
	{
		ApplyPatch(lobbyConnectSig, serverFullOffset, &serverFullPatch, /*restore*/NULL);
	}
	L4D_DEBUG_LOG("PlayerSlots -- 'ValveRejectServerFullFirst' patched to CMP eax, %d", PLAYER_SLOTS_MAX);
//	L4D_DEBUG_LOG("PlayerSlots -- CBaseServer player limit: %d", *(unsigned int *)((int)g_pServer+ 0x17C));
}

void PlayerSlots::UnpatchGetMaxHumanPlayers()
{
	if(getMaxHumanPlayersSig)
	{
		ApplyPatch(getMaxHumanPlayersSig, /*offset*/0, &getMaxHumanPlayersRestore, /*restore*/NULL);
		L4D_DEBUG_LOG("PlayerSlots -- 'GetMaxHumanPlayers' restored");
	}
}

/* Unpatch for Group Lobby - Future implementation - XBetaAlpha

void PlayerSlots::UnpatchGetTotalNumPlayersSupported()
{
        if(getTotalNumPlayersSupportedSig)
        {
                ApplyPatch(getTotalNumPlayersSupportedSig, 0, &getTotalNumPlayersSupportedRestore, NULL);
                L4D_DEBUG_LOG("PlayerSlots -- 'GetTotalNumPlayersSupported' restored");
        }
}

* EOP */

void PlayerSlots::UnpatchSlotCheckOnly()
{
	//cmp around the string "#Valve_Reject_Server_Full"

	if(lobbyConnectSig && serverFullOffset != -1)
	{
		ApplyPatch(lobbyConnectSig, serverFullOffset, &serverFullRestore, /*restore*/NULL);
		L4D_DEBUG_LOG("PlayerSlots -- 'ValveRejectServerFullFirst' restored");
	}
}

void PlayerSlots::Unpatch()
{
	L4D_DEBUG_LOG("PlayersSlots - Unpatching ...");

	//jl around the string "Human player limit reached (%d/%d)"
	// Windows only

#if defined PLATFORM_WINDOWS
	if(humanLimitSig)
	{
		ApplyPatch(humanLimitSig, /*offset*/0, &humanLimitRestore, /*restore*/NULL);
		L4D_DEBUG_LOG("PlayerSlots -- 'HumanPlayerLimitReached' jl(e) restored");
	} 
#endif

	//jz around the string "#Valve_Reject_Server_Full"

	if(lobbyConnectSig)
	{
		ApplyPatch(lobbyConnectSig, /*offset*/0, &lobbyConnectRestore, /*restore*/NULL);
		L4D_DEBUG_LOG("PlayerSlots -- 'ConnectClientLobbyCheck' restored");
	}

	PlayerSlots::UnpatchSlotCheckOnly();
	PlayerSlots::UnpatchGetMaxHumanPlayers();
	//PlayerSlots::UnpatchGetTotalNumPlayersSupported();
}

void OnMaxPlayersChanged( IConVar *var, const char *pOldValue, float flOldValue )
{
	L4D_DEBUG_LOG("CVAR l4d_maxplayers changed to %d...", g_MaxPlayers.GetInt());
	PlayerSlots::OnMaxSlotsChanged(g_MaxPlayers.GetInt());
}

static void UpdateMaxSlots(int max_slots)
{
	PlayerSlots::MaxSlots = max_slots;
	g_MaxPlayers.SetValue(max_slots);
}

void PlayerSlots::OnMaxSlotsChanged(int max_slots)
{
	if(MaxClients == -1)
	{
	    L4D_DEBUG_LOG("MaxClients -1! Disallowing slots patch");
		UpdateMaxSlots(-1);
		return;
	}

	/* no change */

	if(MaxSlots == max_slots)
		return;

	// disable

	if(max_slots < 0)
	{
		L4D_DEBUG_LOG("Disabling slots patch");
		PlayerSlots::Unpatch();
		UpdateMaxSlots(-1);
		return;
	}

#if RESTRICT_MAX_PLAYERS_BY_COMMAND_LINE
	/* dont allow slots to exceed +-maxplayers */

	int max_players = PlayerSlots::MaxPlayers;

	if(max_players >= 0 && max_slots > max_players)
	{
	    L4D_DEBUG_LOG("Attempt to set slots higher than set command line value (%d max: %d)", max_slots, max_players);
		UpdateMaxSlots(MaxSlots);
		return;
	}
#endif

	// cant allow this obviously

	if(max_slots > MaxClients)
	{
  	    L4D_DEBUG_LOG("Attempt to set slots higher than MaxClients (%d max: %d)", max_slots, MaxClients);
		UpdateMaxSlots(MaxSlots);
		return;
	}

	//enable

	UpdateMaxSlots(max_slots);

	L4D_DEBUG_LOG("Enabling slots patch");
	PlayerSlots::Patch();
	PlayerSlots::PatchSlotCheckOnly();
	PlayerSlots::PatchGetMaxHumanPlayers();
	//PlayerSlots::PatchGetTotalNumPlayersSupported();
}
