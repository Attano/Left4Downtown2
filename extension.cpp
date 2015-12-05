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

#include <string.h>

#include "extension.h"
#include "iplayerinfo.h"
#include "vglobals.h"
#include "util.h"
#ifdef USE_PLAYERSLOTS_PATCHES
#include "player_slots.h"
#endif
#include "convar_public.h"

#include <ISDKTools.h>

#include "codepatch/patchmanager.h"
#include "codepatch/autopatch.h"

#include "detours/spawn_special.h"
#include "detours/spawn_witch.h"
#include "detours/spawn_witchbride.h"
#include "detours/spawn_tank.h"
#include "detours/clear_team_scores.h"
#include "detours/set_campaign_scores.h"
#ifdef USE_PLAYERSLOTS_PATCHES
#include "detours/server_player_counts.h"
#endif
#include "detours/first_survivor_left_safe_area.h"
#include "detours/get_script_value_int.h"
#include "detours/get_script_value_float.h"
#include "detours/get_script_value_string.h"
#include "detours/is_finale.h"
#include "detours/on_enter_ghost_state.h"
#include "detours/try_offering_tank_bot.h"
#include "detours/mob_rush_start.h"
#include "detours/spawn_it_mob.h"
#include "detours/spawn_mob.h"
#include "detours/shoved_by_survivor.h"
#include "detours/get_crouch_top_speed.h"
#include "detours/get_run_top_speed.h"
#include "detours/get_walk_top_speed.h"
#include "detours/has_configurable_difficulty.h"
#include "detours/get_survivor_set.h"
#include "detours/fast_get_survivor_set.h"
#include "detours/get_mission_versus_boss_spawning.h"
#include "detours/cthrow_activate_ability.h"
#include "detours/start_melee_swing.h"
#include "detours/use_healing_items.h"
#include "detours/find_scavenge_item.h"
#include "detours/send_in_rescue_vehicle.h"
#include "detours/change_finale_stage.h"
#include "detours/end_versus_mode_round.h"
#include "detours/select_weighted_sequence.h"//for SelectTankAttack
#include "detours/on_revived.h"
#include "detours/water_move.h"
#include "detours/on_stagger.h"
#include "detours/terror_weapon_hit.h"
#include "detours/get_mission_info.h"
#include "detours/inferno_spread.h"
#include "detours/shoved_by_pounce_landing.h"

#include "addons_disabler.h"

#define GAMECONFIG_FILE "left4downtown.l4d2"

Left4Downtown g_Left4DowntownTools;		/**< Global singleton for extension's main interface */
IGameConfig *g_pGameConf = NULL;
IGameConfig *g_pGameConfSDKTools = NULL;
IBinTools *g_pBinTools = NULL;
IServer *g_pServer = NULL; //ptr to CBaseServer
ISDKTools *g_pSDKTools = NULL;
IServerGameEnts *gameents = NULL;
CGlobalVars *gpGlobals;

IForward *g_pFwdOnSpawnSpecial = NULL;
IForward *g_pFwdOnSpawnTank = NULL;
IForward *g_pFwdOnSpawnWitch = NULL;
IForward *g_pFwdOnSpawnWitchBride = NULL;
IForward *g_pFwdOnClearTeamScores = NULL;
IForward *g_pFwdOnSetCampaignScores = NULL;
IForward *g_pFwdOnFirstSurvivorLeftSafeArea = NULL;
IForward *g_pFwdOnGetScriptValueInt = NULL;
IForward *g_pFwdOnGetScriptValueFloat = NULL;
IForward *g_pFwdOnGetScriptValueString = NULL;
IForward *g_pFwdOnTryOfferingTankBot = NULL;
IForward *g_pFwdOnMobRushStart = NULL;
IForward *g_pFwdOnSpawnITMob = NULL;
IForward *g_pFwdOnSpawnMob = NULL;
IForward *g_pFwdOnShovedBySurvivor = NULL;
IForward *g_pFwdOnGetCrouchTopSpeed = NULL;
IForward *g_pFwdOnGetRunTopSpeed = NULL;
IForward *g_pFwdOnGetWalkTopSpeed = NULL;
IForward *g_pFwdOnHasConfigurableDifficulty = NULL;
IForward *g_pFwdOnGetSurvivorSet = NULL;
IForward *g_pFwdOnFastGetSurvivorSet = NULL;
IForward *g_pFwdOnGetMissionVersusBossSpawning = NULL;
IForward *g_pFwdOnCThrowActivate = NULL;
IForward *g_pFwdOnStartMeleeSwing = NULL;
IForward *g_pFwdOnUseHealingItems = NULL;
IForward *g_pFwdOnFindScavengeItem = NULL;
IForward *g_pFwdOnSendInRescueVehicle = NULL;
IForward *g_pFwdOnChangeFinaleStage = NULL;
IForward *g_pFwdOnEndVersusModeRound = NULL;
IForward *g_pFwdOnEndVersusModeRound_Post = NULL;
IForward *g_pFwdOnSelectTankAttack = NULL;
IForward *g_pFwdOnRevived = NULL;
IForward *g_pFwdOnWaterMove = NULL;
IForward *g_pFwdOnPlayerStagger = NULL;
IForward *g_pFwdOnTerrorWeaponHit = NULL;
IForward *g_pFwdAddonsDisabler = NULL;
IForward *g_pFwdInfernoSpread = NULL;
IForward *g_pFwdOnShovedByPounceLanding = NULL;

bool g_bRoundEnd = false;

ICvar *icvar = NULL;
SMEXT_LINK(&g_Left4DowntownTools);

extern sp_nativeinfo_t g_L4DoNatives[];
extern sp_nativeinfo_t g_L4DoTimerNatives[];
extern sp_nativeinfo_t g_L4DoWeaponNatives[];
extern sp_nativeinfo_t g_L4DoMeleeWeaponNatives[];
extern sp_nativeinfo_t g_L4DoDirectorNatives[];

ConVar g_Version("left4downtown_version", SMEXT_CONF_VERSION, FCVAR_SPONLY|FCVAR_NOTIFY, "Left 4 Downtown Extension Version");
ConVar g_AddonsEclipse("l4d2_addons_eclipse", "-1", FCVAR_SPONLY|FCVAR_NOTIFY, "Addons Manager(-1: use addonconfig; 0/1: override addonconfig");
ConVar g_UnlockMelees("l4d2_unlock_melees", "0", FCVAR_SPONLY|FCVAR_NOTIFY, "1: Unlock all melees, 0: don't (set to 0 if you're experiencing crashes on modes other than versus)");
#ifdef USE_PLAYERSLOTS_PATCHES
ConVar g_MaxPlayers("l4d_maxplayers", "-1", FCVAR_SPONLY|FCVAR_NOTIFY, "Overrides maxplayers with this value");
#endif
PatchManager g_PatchManager;

/**
 * @file extension.cpp
 * @brief Implement extension code here.
 */

bool Left4Downtown::SDK_OnLoad(char *error, size_t maxlength, bool late)
{
	//only load extension for l4d2
	if (strcmp(g_pSM->GetGameFolderName(), "left4dead2") != 0)
	{
		UTIL_Format(error, maxlength, "Cannot Load Left 4 Downtown Ext on mods other than L4D2");
		return false;
	}


	//load sigscans and offsets, etc from our gamedata file
	char conf_error[255] = "";
	if (!gameconfs->LoadGameConfigFile(GAMECONFIG_FILE, &g_pGameConf, conf_error, sizeof(conf_error)))
	{
		if (conf_error[0])
		{
			UTIL_Format(error, maxlength, "Could not read " GAMECONFIG_FILE ".txt: %s", conf_error);
		}
		return false;
	}

	//load sigscans and offsets from the sdktools gamedata file
	if (!gameconfs->LoadGameConfigFile("sdktools.games", &g_pGameConfSDKTools, conf_error, sizeof(conf_error)))
	{
		return false;
	}
	sharesys->AddDependency(myself, "bintools.ext", true, true);
	sharesys->RegisterLibrary(myself, "left4downtown2");
	sharesys->AddNatives(myself, g_L4DoNatives);
	sharesys->AddNatives(myself, g_L4DoTimerNatives);
	sharesys->AddNatives(myself, g_L4DoWeaponNatives);
	sharesys->AddNatives(myself, g_L4DoMeleeWeaponNatives);
	sharesys->AddNatives(myself, g_L4DoDirectorNatives);

	g_pFwdOnSpawnSpecial = forwards->CreateForward("L4D_OnSpawnSpecial", ET_Event, 3, /*types*/NULL, Param_CellByRef, Param_Array, Param_Array);
	g_pFwdOnSpawnTank = forwards->CreateForward("L4D_OnSpawnTank", ET_Event, 2, /*types*/NULL, Param_Array, Param_Array);
	g_pFwdOnSpawnWitch = forwards->CreateForward("L4D_OnSpawnWitch", ET_Event, 2, /*types*/NULL, Param_Array, Param_Array);
	g_pFwdOnSpawnWitchBride = forwards->CreateForward("L4D_OnSpawnWitchBride", ET_Event, 2, /*types*/NULL, Param_Array, Param_Array);
	g_pFwdOnClearTeamScores = forwards->CreateForward("L4D_OnClearTeamScores", ET_Event, 1, /*types*/NULL, Param_Cell);
	g_pFwdOnSetCampaignScores = forwards->CreateForward("L4D_OnSetCampaignScores", ET_Event, 2, /*types*/NULL, Param_CellByRef, Param_CellByRef);
	g_pFwdOnFirstSurvivorLeftSafeArea = forwards->CreateForward("L4D_OnFirstSurvivorLeftSafeArea", ET_Event, 1, /*types*/NULL, Param_Cell);
	g_pFwdOnGetScriptValueInt = forwards->CreateForward("L4D_OnGetScriptValueInt", ET_Event, 2, /*types*/NULL, Param_String, Param_CellByRef);
	g_pFwdOnGetScriptValueFloat = forwards->CreateForward("L4D_OnGetScriptValueFloat", ET_Event, 2, /*types*/NULL, Param_String, Param_FloatByRef);
	g_pFwdOnGetScriptValueString = forwards->CreateForward("L4D_OnGetScriptValueString", ET_Event, 4, /*types*/NULL, Param_String, Param_String, Param_String, Param_CellByRef);
	g_pFwdOnTryOfferingTankBot = forwards->CreateForward("L4D_OnTryOfferingTankBot", ET_Event, 2, /*types*/NULL, Param_Cell, Param_CellByRef);
	g_pFwdOnMobRushStart = forwards->CreateForward("L4D_OnMobRushStart", ET_Event, 0, /*types*/NULL);
	g_pFwdOnSpawnITMob = forwards->CreateForward("L4D_OnSpawnITMob", ET_Event, 1, /*types*/NULL, Param_CellByRef);
	g_pFwdOnSpawnMob = forwards->CreateForward("L4D_OnSpawnMob", ET_Event, 1, /*types*/NULL, Param_CellByRef);
	g_pFwdOnShovedBySurvivor = forwards->CreateForward("L4D_OnShovedBySurvivor", ET_Event, 3, /*types*/NULL, Param_Cell, Param_Cell, Param_Array);
	g_pFwdOnGetCrouchTopSpeed = forwards->CreateForward("L4D_OnGetCrouchTopSpeed", ET_Event, 2, /*types*/NULL, Param_Cell, Param_FloatByRef);
	g_pFwdOnGetRunTopSpeed = forwards->CreateForward("L4D_OnGetRunTopSpeed", ET_Event, 2, /*types*/NULL, Param_Cell, Param_FloatByRef);
	g_pFwdOnGetWalkTopSpeed = forwards->CreateForward("L4D_OnGetWalkTopSpeed", ET_Event, 2, /*types*/NULL, Param_Cell, Param_FloatByRef);
	g_pFwdOnHasConfigurableDifficulty = forwards->CreateForward("L4D_OnHasConfigurableDifficulty", ET_Event, 1, /*types*/NULL, Param_CellByRef);
	g_pFwdOnGetSurvivorSet = forwards->CreateForward("L4D_OnGetSurvivorSet", ET_Event, 1, /*types*/NULL, Param_CellByRef);
	g_pFwdOnFastGetSurvivorSet = forwards->CreateForward("L4D_OnFastGetSurvivorSet", ET_Event, 1, /*types*/NULL, Param_CellByRef);
	g_pFwdOnGetMissionVersusBossSpawning = forwards->CreateForward("L4D_OnGetMissionVSBossSpawning", ET_Event, 4, /*types*/NULL, Param_FloatByRef, Param_FloatByRef, Param_FloatByRef, Param_FloatByRef);
	g_pFwdOnCThrowActivate = forwards->CreateForward("L4D_OnCThrowActivate", ET_Event, 1, /*types*/NULL, Param_Cell);
	g_pFwdOnStartMeleeSwing = forwards->CreateForward("L4D_OnStartMeleeSwing", ET_Event, 2, /*types*/NULL, Param_Cell, Param_Cell);
	g_pFwdOnUseHealingItems = forwards->CreateForward("L4D2_OnUseHealingItems", ET_Event, 1, /*types*/NULL, Param_Cell);
	g_pFwdOnFindScavengeItem = forwards->CreateForward("L4D2_OnFindScavengeItem", ET_Event, 2, /*types*/NULL, Param_Cell, Param_CellByRef);
	g_pFwdOnSendInRescueVehicle = forwards->CreateForward("L4D2_OnSendInRescueVehicle", ET_Event, 0, /*types*/NULL);
	g_pFwdOnChangeFinaleStage = forwards->CreateForward("L4D2_OnChangeFinaleStage", ET_Event, 2, /*types*/NULL, Param_CellByRef, Param_String);
	g_pFwdOnEndVersusModeRound = forwards->CreateForward("L4D2_OnEndVersusModeRound", ET_Event, 1, /*types*/NULL, Param_Cell);
	g_pFwdOnEndVersusModeRound_Post = forwards->CreateForward("L4D2_OnEndVersusModeRound_Post", ET_Ignore, 0, /*types*/NULL);
	g_pFwdOnSelectTankAttack = forwards->CreateForward("L4D2_OnSelectTankAttack", ET_Event, 2, /*types*/NULL, Param_Cell, Param_CellByRef);
	g_pFwdOnRevived = forwards->CreateForward("L4D_OnRevived", ET_Event, 1, /*types*/NULL, Param_Cell);
    g_pFwdOnWaterMove = forwards->CreateForward("L4D2_OnWaterMove", ET_Event, 1, /*types*/NULL, Param_Cell);
	g_pFwdOnPlayerStagger = forwards->CreateForward("L4D2_OnStagger", ET_Event, 2, /*types*/NULL, Param_Cell, Param_Cell);
	g_pFwdOnTerrorWeaponHit = forwards->CreateForward("L4D2_OnEntityShoved", ET_Event, 5, /*types*/NULL, Param_Cell, Param_Cell, Param_Cell, Param_Array, Param_Cell);
	g_pFwdAddonsDisabler = forwards->CreateForward("L4D2_OnClientDisableAddons", ET_Event, 1, /*types*/NULL, Param_String);
	g_pFwdInfernoSpread = forwards->CreateForward("L4D2_OnSpitSpread", ET_Event, 5, /*types*/NULL, Param_Cell, Param_Cell, Param_FloatByRef, Param_FloatByRef, Param_FloatByRef);
	g_pFwdOnShovedByPounceLanding = forwards->CreateForward("L4D2_OnPounceOrLeapStumble", ET_Event, 2, /*types*/NULL, Param_Cell, Param_Cell);

	playerhelpers->AddClientListener(&g_Left4DowntownTools);
	playerhelpers->RegisterCommandTargetProcessor(&g_Left4DowntownTools);

	Detour::Init(g_pSM->GetScriptingEngine(), g_pGameConf);

	return true;
}

void Left4Downtown::SDK_OnAllLoaded()
{
	SM_GET_LATE_IFACE(BINTOOLS, g_pBinTools);
	SM_GET_LATE_IFACE(SDKTOOLS, g_pSDKTools);

	if (!g_pBinTools || !g_pSDKTools)
	{
		L4D_DEBUG_LOG("Failed to load bintools or failed to load sdktools");
		return;
	}

	IServer *server = g_pSDKTools->GetIServer();
	L4D_DEBUG_LOG("Address of IServer is %p", server);
	//reading out server->GetName() we consistently seem to get (the same?) 
	//garbage characters. this is possibly causing a crash on windows servers
	//when a player connects. so lets not read the server name :(
	//L4D_DEBUG_LOG("Server name is %s", server->GetName());
	g_pServer = server;

	InitializeValveGlobals();

    g_AddonsEclipse.InstallChangeCallback(::OnAddonsEclipseChanged);
    AddonsDisabler::AddonsEclipse = g_AddonsEclipse.GetInt();

#ifdef USE_PLAYERSLOTS_PATCHES
	/*
		allow more than 8 players in l4d if l4d_maxplayers is not -1
		also if +maxplayers or -maxplayers is not the default value (8)
	*/
	g_MaxPlayers.InstallChangeCallback(::OnMaxPlayersChanged);

	/*
	read the +-maxplayers from command line
	*/

	/*commenting out the code below
	seems to not make it crash during connection to my NFO
	
	START HERE
	*/
	ICommandLine *cmdLine = CommandLine_Tier0();
	int maxplayers = -1;
	L4D_DEBUG_LOG("Command line is: %s", cmdLine->GetCmdLine());

	maxplayers = cmdLine->ParmValue("+maxplayers", -1);
	L4D_DEBUG_LOG("Command line +maxplayers is: %d", maxplayers);

	if(maxplayers == -1) 
	{
		maxplayers = cmdLine->ParmValue("-maxplayers", -1);
	}
	/*
	end here
	*/
	PlayerSlots::MaxPlayers = maxplayers;
#endif

	/*
	detour the witch/spawn spawns
	*/
	//automatically will unregister/cleanup themselves when the ext is unloaded

	g_PatchManager.Register(new AutoPatch<Detours::SpawnSpecial>());
	g_PatchManager.Register(new AutoPatch<Detours::SpawnTank>());
	g_PatchManager.Register(new AutoPatch<Detours::SpawnWitch>());
	g_PatchManager.Register(new AutoPatch<Detours::SpawnWitchBride>());
	g_PatchManager.Register(new AutoPatch<Detours::ClearTeamScores>());
	g_PatchManager.Register(new AutoPatch<Detours::SetCampaignScores>());

	g_PatchManager.Register(new AutoPatch<Detours::FirstSurvivorLeftSafeArea>());
	g_PatchManager.Register(new AutoPatch<Detours::GetScriptValueInt>());
	g_PatchManager.Register(new AutoPatch<Detours::GetScriptValueFloat>());
	g_PatchManager.Register(new AutoPatch<Detours::GetScriptValueString>());
	g_PatchManager.Register(new AutoPatch<Detours::TryOfferingTankBot>());
	g_PatchManager.Register(new AutoPatch<Detours::MobRushStart>());
	g_PatchManager.Register(new AutoPatch<Detours::SpawnITMob>());
	g_PatchManager.Register(new AutoPatch<Detours::SpawnMob>());
	g_PatchManager.Register(new AutoPatch<Detours::ShovedBySurvivor>());
	g_PatchManager.Register(new AutoPatch<Detours::GetCrouchTopSpeed>());
	g_PatchManager.Register(new AutoPatch<Detours::GetRunTopSpeed>());
	g_PatchManager.Register(new AutoPatch<Detours::GetWalkTopSpeed>());
	g_PatchManager.Register(new AutoPatch<Detours::HasConfigurableDifficulty>());
	g_PatchManager.Register(new AutoPatch<Detours::GetSurvivorSet>());
	g_PatchManager.Register(new AutoPatch<Detours::FastGetSurvivorSet>());
	g_PatchManager.Register(new AutoPatch<Detours::GetMissionVersusBossSpawning>());
	g_PatchManager.Register(new AutoPatch<Detours::CThrowActivate>());
	g_PatchManager.Register(new AutoPatch<Detours::StartMeleeSwing>());
	g_PatchManager.Register(new AutoPatch<Detours::UseHealingItems>());
	g_PatchManager.Register(new AutoPatch<Detours::FindScavengeItem>());
	g_PatchManager.Register(new AutoPatch<Detours::SendInRescueVehicle>());
	g_PatchManager.Register(new AutoPatch<Detours::ChangeFinaleStage>());
	g_PatchManager.Register(new AutoPatch<Detours::EndVersusModeRound>());
	g_PatchManager.Register(new AutoPatch<Detours::SelectWeightedSequence>());//for SelectTankAttack
	g_PatchManager.Register(new AutoPatch<Detours::Revived>());
    g_PatchManager.Register(new AutoPatch<Detours::WaterMove>());
	g_PatchManager.Register(new AutoPatch<Detours::PlayerStagger>());
	g_PatchManager.Register(new AutoPatch<Detours::TerrorWeaponHit>());
    g_PatchManager.Register(new AutoPatch<Detours::CTerrorGameRules>());
	g_PatchManager.Register(new AutoPatch<Detours::CBaseServer>());
	g_PatchManager.Register(new AutoPatch<Detours::InfernoSpread>());
	g_PatchManager.Register(new AutoPatch<Detours::ShovedByPounceLanding>());

	//new style detours that create/destroy the forwards themselves
	g_PatchManager.Register(new AutoPatch<Detours::IsFinale>());
	g_PatchManager.Register(new AutoPatch<Detours::OnEnterGhostState>());
	#ifdef USE_PLAYERSLOTS_PATCHES
	g_PatchManager.Register(new AutoPatch<Detours::ServerPlayerCounts>());
	#endif
}

void Left4Downtown::SDK_OnUnload()
{
	gameconfs->CloseGameConfigFile(g_pGameConf);
	gameconfs->CloseGameConfigFile(g_pGameConfSDKTools);

	playerhelpers->RemoveClientListener(&g_Left4DowntownTools);
	playerhelpers->UnregisterCommandTargetProcessor(&g_Left4DowntownTools);

#ifdef USE_PLAYERSLOTS_PATCHES
	//go back to normal old asm 
	PlayerSlots::Unpatch();
#endif

    AddonsDisabler::Unpatch();

	g_PatchManager.UnregisterAll();

	forwards->ReleaseForward(g_pFwdOnSpawnSpecial);
	forwards->ReleaseForward(g_pFwdOnSpawnTank);
	forwards->ReleaseForward(g_pFwdOnSpawnWitch);
	forwards->ReleaseForward(g_pFwdOnSpawnWitchBride);
	forwards->ReleaseForward(g_pFwdOnClearTeamScores);
	forwards->ReleaseForward(g_pFwdOnSetCampaignScores);

	forwards->ReleaseForward(g_pFwdOnFirstSurvivorLeftSafeArea);
	forwards->ReleaseForward(g_pFwdOnGetScriptValueInt);
	forwards->ReleaseForward(g_pFwdOnGetScriptValueFloat);
	forwards->ReleaseForward(g_pFwdOnGetScriptValueString);
	forwards->ReleaseForward(g_pFwdOnTryOfferingTankBot);
	forwards->ReleaseForward(g_pFwdOnMobRushStart);
	forwards->ReleaseForward(g_pFwdOnSpawnITMob);
	forwards->ReleaseForward(g_pFwdOnSpawnMob);
	forwards->ReleaseForward(g_pFwdOnShovedBySurvivor);
	forwards->ReleaseForward(g_pFwdOnGetCrouchTopSpeed);
	forwards->ReleaseForward(g_pFwdOnGetRunTopSpeed);
	forwards->ReleaseForward(g_pFwdOnGetWalkTopSpeed);
	forwards->ReleaseForward(g_pFwdOnHasConfigurableDifficulty);
	forwards->ReleaseForward(g_pFwdOnGetSurvivorSet);
	forwards->ReleaseForward(g_pFwdOnFastGetSurvivorSet);
	forwards->ReleaseForward(g_pFwdOnGetMissionVersusBossSpawning);
	forwards->ReleaseForward(g_pFwdOnCThrowActivate);
	forwards->ReleaseForward(g_pFwdOnStartMeleeSwing);
	forwards->ReleaseForward(g_pFwdOnUseHealingItems);
	forwards->ReleaseForward(g_pFwdOnFindScavengeItem);
	forwards->ReleaseForward(g_pFwdOnSendInRescueVehicle);
	forwards->ReleaseForward(g_pFwdOnChangeFinaleStage);
	forwards->ReleaseForward(g_pFwdOnEndVersusModeRound);
	forwards->ReleaseForward(g_pFwdOnEndVersusModeRound_Post);
	forwards->ReleaseForward(g_pFwdOnSelectTankAttack);
	forwards->ReleaseForward(g_pFwdOnRevived);
    forwards->ReleaseForward(g_pFwdOnWaterMove);
	forwards->ReleaseForward(g_pFwdOnPlayerStagger);
	forwards->ReleaseForward(g_pFwdOnTerrorWeaponHit);
    forwards->ReleaseForward(g_pFwdAddonsDisabler);
    forwards->ReleaseForward(g_pFwdInfernoSpread);
    forwards->ReleaseForward(g_pFwdOnShovedByPounceLanding);
}

class BaseAccessor : public IConCommandBaseAccessor
{
public:
	bool RegisterConCommandBase(ConCommandBase *pCommandBase)
	{
		/* Always call META_REGCVAR instead of going through the engine. */
		return META_REGCVAR(pCommandBase);
	}
} s_BaseAccessor;


bool Left4Downtown::SDK_OnMetamodLoad(SourceMM::ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
	GET_V_IFACE_CURRENT(GetEngineFactory, icvar, ICvar, CVAR_INTERFACE_VERSION);

	g_pCVar = icvar;
	ConVar_Register(0, &s_BaseAccessor);

	GET_V_IFACE_ANY(GetServerFactory, gameents, IServerGameEnts, INTERFACEVERSION_SERVERGAMEENTS);
	gpGlobals = ismm->GetCGlobals();

	return true;
}
#ifdef USE_PLAYERSLOTS_PATCHES
	/**
	 * @brief Called when the server is activated.
	 */
void Left4Downtown::OnServerActivated(int max_clients)
{
	L4D_DEBUG_LOG("Server activated with %d maxclients", max_clients);

	static bool firstTime = true;
	if(!firstTime)
	{
		L4D_DEBUG_LOG("Server activated with %d maxclients (doing nothing)", max_clients);
		return;
	}
	firstTime = false;

	PlayerSlots::MaxClients = max_clients;
	
	ConVarPublic *maxPlayersCvar = (ConVarPublic*)&g_MaxPlayers;
	maxPlayersCvar->m_bHasMax = true;
	maxPlayersCvar->m_fMaxVal = (float) max_clients;
	maxPlayersCvar->m_bHasMin  = true;
	maxPlayersCvar->m_fMinVal = (float) -1;

#if RESTRICT_MAX_PLAYERS_BY_COMMAND_LINE
	int max_players = PlayerSlots::MaxPlayers;

	if(max_players >= 0)
	{
		//dont allow it to be larger than max_clients
		max_players = max_players <= max_clients ? max_players : max_clients;

		maxPlayersCvar->m_fMaxVal = (float) max_players; 

		//if GSPs set maxplayers to the non-default value
		//we will patch the code then by default even if l4d_maxplayers is never set
		//otherwise default is -1 disabled
		if(DEFAULT_MAX_PLAYERS != max_players)
		{
			//by putting only +-maxplayers and never l4d_maxplayers, we set maxplayers +-maxplayers value
			//by specifiying l4d_maxplayers, we override the +-maxplayers value as long as its <= +-maxplayers
			g_MaxPlayers.SetValue(max_players);

			static char defaultPlayers[5];
			snprintf(defaultPlayers, sizeof(defaultPlayers), "%d", max_players);
			maxPlayersCvar->m_pszDefaultValue = defaultPlayers;
		}
	}
#endif
}
#endif

bool Left4Downtown::ProcessCommandTarget(cmd_target_info_t *info)
{
	int max_clients;
	IPlayerInfo *pInfo;
	unsigned int team_index = 0;
	IGamePlayer *pPlayer, *pAdmin;

	if ((info->flags & COMMAND_FILTER_NO_MULTI) == COMMAND_FILTER_NO_MULTI)
	{
		return false;
	}

	if (info->admin)
	{
		if ((pAdmin = playerhelpers->GetGamePlayer(info->admin)) == NULL)
		{
			return false;
		}
		if (!pAdmin->IsInGame())
		{
			return false;
		}
	}
	else
	{
		pAdmin = NULL;
	}

	if (strcmp(info->pattern, "@survivors") == 0 )
	{
		team_index = 2;
	}
	else if (strcmp(info->pattern, "@infected") == 0)
	{
		team_index = 3;
	}
	else
	{
		return false;
	}

	info->num_targets = 0;

	max_clients = playerhelpers->GetMaxClients();
	for (int i = 1; 
		 i <= max_clients && (cell_t)info->num_targets < info->max_targets; 
		 i++)
	{
		if ((pPlayer = playerhelpers->GetGamePlayer(i)) == NULL)
		{
			continue;
		}
		if (!pPlayer->IsInGame())
		{
			continue;
		}
		if ((pInfo = pPlayer->GetPlayerInfo()) == NULL)
		{
			continue;
		}
		if (pInfo->GetTeamIndex() != (int)team_index)
		{
			continue;
		}
		if (playerhelpers->FilterCommandTarget(pAdmin, pPlayer, info->flags) 
			!= COMMAND_TARGET_VALID)
		{
			continue;
		}
		info->targets[info->num_targets] = i;
		info->num_targets++;
	}

	if (info->num_targets == 0)
	{
		info->reason = COMMAND_TARGET_EMPTY_FILTER;
	}
	else
	{
		info->reason = COMMAND_TARGET_VALID;
	}

	info->target_name_style = COMMAND_TARGETNAME_RAW;
	if (team_index == 2)
	{
		UTIL_Format(info->target_name, info->target_name_maxlength, "Survivors");
	}
	else if (team_index == 3)
	{
		UTIL_Format(info->target_name, info->target_name_maxlength, "Infected");
	}

	return true;
}
