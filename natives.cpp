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
#include "vglobals.h"
#include "util.h"
#include "l4d2sdk/constants.h"

// native L4D_GetTeamScore(logical_team, campaign_score=false)
cell_t L4D_GetTeamScore(IPluginContext *pContext, const cell_t *params)
{
	static ICallWrapper *pWrapper = NULL;

	// CTerrorGameRules::GetTeamScore(int,bool)
	if (!pWrapper)
	{
		PassInfo ret;
		ret.flags = PASSFLAG_BYVAL;
		ret.size = sizeof(int);
		ret.type = PassType_Basic;

		REGISTER_NATIVE_ADDR("GetTeamScore", 
			PassInfo pass[2]; \
			pass[0].flags = PASSFLAG_BYVAL; \
			pass[0].size = sizeof(int); \
			pass[0].type = PassType_Basic; \
			pass[1].flags = PASSFLAG_BYVAL; \
			pass[1].size = sizeof(bool); \
			pass[1].type = PassType_Basic; \
			pWrapper = g_pBinTools->CreateCall(addr, CallConv_ThisCall, &ret, pass, /*numparams*/2));
	}

#define SCORE_TEAM_A 1
#define SCORE_TEAM_B 2
#define SCORE_TYPE_ROUND 0
#define SCORE_TYPE_CAMPAIGN 1

	//sanity check that the team index is valid
	if(params[1] != SCORE_TEAM_A && params[1] != SCORE_TEAM_B)
	{
		return pContext->ThrowNativeError("Logical team %d is invalid", params[1]);
	}
	//campaign_score is a boolean so should be 0 (use round score) or 1 only 
	if(params[2] != SCORE_TYPE_ROUND && params[2] != SCORE_TYPE_CAMPAIGN)
	{
		return pContext->ThrowNativeError("campaign_score %d is invalid, needs to be 0 or 1", params[2]);
	}

	void *gamerules = g_pSDKTools->GetGameRules();

	if (gamerules == NULL)
	{
		return pContext->ThrowNativeError("GameRules not available before map is loaded");
	}

	/* Build the vcall argument stack */
	unsigned char vstk[sizeof(void *) + sizeof(int) + sizeof(bool)];
	unsigned char *vptr = vstk;

	*(void **)vptr = gamerules;
	vptr += sizeof(void *);
	*(int *)vptr = params[1];
	vptr += sizeof(int);
	*(bool *)vptr = params[2] ? true : false;

	cell_t retbuffer;
	pWrapper->Execute(vstk, &retbuffer);

	return retbuffer;
}

// native L4D_RestartScenarioFromVote(const String:map[])
cell_t L4D_RestartScenarioFromVote(IPluginContext *pContext, const cell_t *params)
{
	static ICallWrapper *pWrapper = NULL;

	// Director::RestartScenario()
	if (!pWrapper)
	{
		REGISTER_NATIVE_ADDR("RestartScenarioFromVote", 
			PassInfo pass[1]; \
			pass[0].flags = PASSFLAG_BYVAL; \
			pass[0].size = sizeof(char *); \
			pass[0].type = PassType_Basic; \
			pWrapper = g_pBinTools->CreateCall(addr, CallConv_ThisCall, /*returnInfo*/NULL, pass, /*numparams*/1));
	}

#ifdef PLATFORM_WINDOWS
	void *addr;
	if (!g_pGameConf->GetMemSig("RestartScenarioFromVote", (void **)&addr) || !addr)
	{
		return pContext->ThrowNativeError( "Could not read RestartScenarioFromVote from GameConf");
	}

	int offset;
	if (!g_pGameConf->GetOffset("TheDirector", &offset) || !offset)
	{
#if !defined THEDIRECTOR_SETNEXTMISSION_OFFSET
		return pContext->ThrowNativeError("Could not read 'TheDirector' offset from GameConf");
#endif
	}
#endif

	/* Get the Director pointer */
	if (g_pDirector == NULL)
	{
		return pContext->ThrowNativeError("Director unsupported or not available; file a bug report");
	}

	void *director = *g_pDirector;

	if (director == NULL)
	{
		return pContext->ThrowNativeError("Director not available before map is loaded");
	}
	L4D_DEBUG_LOG("Director pointer calculated to be 0x%x", director);

	char *map = NULL;
	pContext->LocalToString(params[1], &map);

	/* Build the vcall argument stack */
	unsigned char vstk[sizeof(void *) + sizeof(char *)];
	unsigned char *vptr = vstk;

	*(void **)vptr = director;
	vptr += sizeof(void *);
	*(char **)vptr = map;
	pWrapper->Execute(vstk, /*retbuffer*/NULL);

	return 1;
}

//DEPRECATED
// native L4D_GetCampaignScores(&scoreA, &scoreB)
cell_t L4D_GetCampaignScores(IPluginContext *pContext, const cell_t *params)
{
	/*
	 Support for this going forward is deprecated in L4D2
	Users can use OnClearTeamScores detour and check for 'true' when campaign scores are reset
	And keep their own campaign tally using GetTeamScore
	*/
	cell_t *addr;
	pContext->LocalToPhysAddr(params[1], &addr);
	*addr = static_cast<cell_t>(-1);

	pContext->LocalToPhysAddr(params[2], &addr);
	*addr = static_cast<cell_t>(-1);

	return 1;
}


// native L4D_LobbyUnreserve()
cell_t L4D_LobbyUnreserve(IPluginContext *pContext, const cell_t *params)
{
	const int64_t cookieUnreserved = 0;

	if(g_pServer == NULL)
	{
		return pContext->ThrowNativeError("CBaseServer not available");
	}

	static ICallWrapper *pWrapper = NULL;

	/* CBaseServer::SetReservationCookie(
			uint64_t reservationCookie,
			const char* formatString,
			va_list ap)

		__cdecl on windows, __thiscall on linux
	*/
	if (!pWrapper)
	{
#ifdef PLATFORM_WINDOWS
		REGISTER_NATIVE_ADDR("SetReservationCookie", 
		PassInfo pass[4]; \
			pass[0].flags = PASSFLAG_BYVAL; \
			pass[0].size = sizeof(IServer *); \
			pass[0].type = PassType_Basic; \
			pass[1].flags = PASSFLAG_BYVAL; \
			pass[1].size = sizeof(uint64_t); \
			pass[1].type = PassType_Basic; \
			pass[2].flags = PASSFLAG_BYVAL; \
			pass[2].size = sizeof(char const*); \
			pass[2].type = PassType_Basic; \
			pass[3].flags = PASSFLAG_BYVAL; \
			pass[3].size = sizeof(void *); \
			pass[3].type = PassType_Basic; \
			pWrapper = g_pBinTools->CreateCall(addr, CallConv_Cdecl, \
							/*retInfo*/NULL, /*paramInfo*/pass, /*numparams*/4));
#else
		REGISTER_NATIVE_ADDR("SetReservationCookie", 
		PassInfo pass[3]; \
			pass[0].flags = PASSFLAG_BYVAL; \
			pass[0].size = sizeof(uint64_t); \
			pass[0].type = PassType_Basic; \
			pass[1].flags = PASSFLAG_BYVAL; \
			pass[1].size = sizeof(char const*); \
			pass[1].type = PassType_Basic; \
			pass[2].flags = PASSFLAG_BYVAL; \
			pass[2].size = sizeof(void *); \
			pass[2].type = PassType_Basic; \
			pWrapper = g_pBinTools->CreateCall(addr, CallConv_ThisCall, \
							/*retInfo*/NULL, /*paramInfo*/pass, /*numparams*/3));
#endif
	}

	/* Build the vcall argument stack */
	unsigned char vstk[sizeof(IServer *) + sizeof(uint64_t) + sizeof(char const *) + sizeof(void *)];
	unsigned char *vptr = vstk;

	*(void **)vptr = g_pServer;
	vptr += sizeof(IServer *);
	*(uint64_t *)vptr = cookieUnreserved;
	vptr += sizeof(uint64_t);
	*(const char **)vptr = "Manually unreserved by Left 4 Downtown Extension";
	vptr += sizeof(char *);
	*(void **)vptr = NULL; // don't bother using a va_list ap :)

	pWrapper->Execute(vstk, /*retbuffer*/NULL);

	L4D_DEBUG_LOG("Invoked CBaseServer::SetReservationCookie(0,fmt,...)");

	return 1;
}

//DEPRECATED ON L4D2 and L4D1 Linux
// native bool:L4D_LobbyIsReserved()
cell_t L4D_LobbyIsReserved(IPluginContext *pContext, const cell_t *params)
{
	g_pSM->LogError(myself, "L4D_LobbyIsReserved() has been called. It is deprecated in L4D2, consider updating the plugin using this native.");

	return 0;
}

// native L4D_ScavengeBeginRoundSetupTime()
cell_t L4D_ScavengeBeginRoundSetupTime(IPluginContext *pContext, const cell_t *params)
{
	static ICallWrapper *pWrapper = NULL;

	// Director::RestartScenario()
	if (!pWrapper)
	{
		REGISTER_NATIVE_ADDR("CDirectorScavengeMode_OnBeginRoundSetupTime", 
			pWrapper = g_pBinTools->CreateCall(addr, CallConv_ThisCall, /*returnInfo*/NULL, /*Pass*/NULL, /*numparams*/0));
	}

	void *addr;
	if (!g_pGameConf->GetMemSig("CDirectorScavengeMode_OnBeginRoundSetupTime", (void **)&addr) || !addr)
	{
		return pContext->ThrowNativeError( "Could not read CDirectorScavengeMode_OnBeginRoundSetupTime from GameConf");
	}

	/* Get the Director pointer */
	if (g_pDirector == NULL)
	{
		return pContext->ThrowNativeError("Director unsupported or not available; file a bug report");
	}

	void *director = *g_pDirector;

	if (director == NULL)
	{
		return pContext->ThrowNativeError("Director not available before map is loaded");
	}

	/* Get the DirectorScavengeMode pointer */
	if (!g_pGameConf->GetMemSig("CDirector_AreWanderersAllowed", (void **)&addr) || !addr)
	{
		return pContext->ThrowNativeError( "Could not read CDirector_AreWanderersAllowed from GameConf");
	}

	//read offset into CDirector_AreWanderersAllowed
	int offset;
	if (!g_pGameConf->GetOffset("CDirectorScavengeMode", &offset) || !offset)
	{
#if !defined THEDIRECTOR_SETNEXTMISSION_OFFSET
		return pContext->ThrowNativeError("Could not read 'CDirectorScavengeMode' offset from GameConf");
#endif
	}

	/* read the offset into director */
	offset = *reinterpret_cast<int*>(reinterpret_cast<uint8_t*>(addr) + offset);
	L4D_DEBUG_LOG("CDirectorScavengeMode director offset calculated to %d", offset);

	void *directorScavengeMode = *reinterpret_cast<void**>(reinterpret_cast<uint8_t*>(director) + offset);
	L4D_DEBUG_LOG("CDirectorScavengeMode pointer is %x", directorScavengeMode);

	if(directorScavengeMode == NULL)
	{
		return pContext->ThrowNativeError("CDirectorScavengeMode unsupported or not available; file a bug report");
	}

	/* Build the vcall argument stack */
	unsigned char vstk[sizeof(void *)];
	unsigned char *vptr = vstk;

	*(void **)vptr = directorScavengeMode;
	pWrapper->Execute(vstk, /*retbuffer*/NULL);

	return 1;
}

cell_t L4D_GetVersusMaxCompletionScore(IPluginContext *pContext, const cell_t *params)
{
	void *gamerules = g_pSDKTools->GetGameRules();
	
	if (gamerules == NULL)
	{
		return pContext->ThrowNativeError("GameRules not available before map is loaded");
	}
	
	int offset;
	if (!g_pGameConf->GetOffset("VersusMaxCompletionScore", &offset) || !offset)
	{
		return pContext->ThrowNativeError("Could not read 'VersusMaxCompletionScore' offset from GameConf");
	}

	L4D_DEBUG_LOG("Reading Versus MaxDistance");
	
	return *(unsigned int *)((unsigned char *)gamerules + offset);
}

cell_t L4D_SetVersusMaxCompletionScore(IPluginContext *pContext, const cell_t *params)
{
	void *gamerules = g_pSDKTools->GetGameRules();

	if (gamerules == NULL)
	{
		return pContext->ThrowNativeError("GameRules not available before map is loaded");
	}
	
	int offset;
	if (!g_pGameConf->GetOffset("VersusMaxCompletionScore", &offset) || !offset)
	{
		return pContext->ThrowNativeError("Could not read 'VersusMaxCompletionScore' offset from GameConf");
	}

	L4D_DEBUG_LOG("Reading Versus MaxDistance");
	
	unsigned int * pVersusMaxCompletionScore = (unsigned int *)((unsigned char *)gamerules + offset);

	*pVersusMaxCompletionScore = params[1];

	return 1;
}

cell_t L4D_IsMissionFinalMap(IPluginContext *pContext, const cell_t *params)
{
	static ICallWrapper *pWrapper = NULL;
	
	if(!pWrapper)
	{
		PassInfo retInfo; 
		retInfo.flags = PASSFLAG_BYVAL; 
#if defined PLATFORM_WINDOWS
		retInfo.size = sizeof(bool);  //ret value in al on windows, eax on linux
#else
		retInfo.size = sizeof(int); //ret value in al on windows, eax on linux
#endif
		retInfo.type = PassType_Basic;
		REGISTER_NATIVE_ADDR("IsMissionFinalMap", 
			pWrapper = g_pBinTools->CreateCall(addr, CallConv_Cdecl, \
							/*retInfo*/&retInfo, /*paramInfo*/NULL, /*numparams*/0));

		L4D_DEBUG_LOG("Built call wrapper CTerrorGameRules::IsMissionFinalMap");
	}
	
	cell_t retbuffer = 0;
	
	L4D_DEBUG_LOG("Going to execute CTerrorGameRules::IsMissionFinalMap");
	pWrapper->Execute(NULL, &retbuffer);
	
	L4D_DEBUG_LOG("Invoked CTerrorGameRules::IsMissionFinalMap, got back = %d", retbuffer);
	
	return retbuffer;
}


// CDirector::ResetMobTimer()
// native L4D_ResetMobTimer()
cell_t L4D_ResetMobTimer(IPluginContext *pContext, const cell_t *params)
{
	static ICallWrapper *pWrapper = NULL;

	// CDirector::ResetMobTimer()
	if (!pWrapper)
	{
		REGISTER_NATIVE_ADDR("ResetMobTimer", 
			pWrapper = g_pBinTools->CreateCall(addr, CallConv_ThisCall, /*returnInfo*/NULL, /*Pass*/NULL, /*numparams*/0));
	}
	
	if (g_pDirector == NULL)
	{
		return pContext->ThrowNativeError("Director unsupported or not available; file a bug report");
	}

	void *director = *g_pDirector;

	if (director == NULL)
	{
		return pContext->ThrowNativeError("Director not available before map is loaded");
	}
	
	/* Build the vcall argument stack */
	unsigned char vstk[sizeof(void *)];
	unsigned char *vptr = vstk;

	*(void **)vptr = director;
	pWrapper->Execute(vstk, /*retbuffer*/NULL);
	
	return 0;
}

//  CGameRulesProxy::NotifyNetworkStateChanged()
// native L4D_NotifyNetworkStateChanged()
cell_t L4D_NotifyNetworkStateChanged(IPluginContext *pContext, const cell_t *params)
{
	static ICallWrapper *pWrapper = NULL;

	// CGameRulesProxy::NotifyNetworkStateChanged()
	if (!pWrapper)
	{
		REGISTER_NATIVE_ADDR("NotifyNetworkStateChanged", 
			pWrapper = g_pBinTools->CreateCall(addr, CallConv_ThisCall, /*returnInfo*/NULL, /*Pass*/NULL, /*numparams*/0));
	}
	
	pWrapper->Execute(NULL, /*retbuffer*/NULL);
	
	return 0;
}

// CTerrorPlayer::OnStaggered(CBaseEntity*, Vector*);
// native L4D_StaggerPlayer(target, source_ent, Float:source_vector[3])
cell_t L4D_StaggerPlayer(IPluginContext *pContext, const cell_t *params)
{
	static ICallWrapper *pWrapper = NULL;

	// CBasePlayer::OnStaggered(CBaseEntity*, Vector*);
	if (!pWrapper)
	{

		REGISTER_NATIVE_ADDR("CTerrorPlayer_OnStaggered", 
			PassInfo pass[2]; \
			pass[0].flags = PASSFLAG_BYVAL; \
			pass[0].size = sizeof(CBaseEntity *); \
			pass[0].type = PassType_Basic; \
			pass[1].flags = PASSFLAG_BYVAL; \
			pass[1].size = sizeof(Vector *); \
			pass[1].type = PassType_Basic; \
			pWrapper = g_pBinTools->CreateCall(addr, CallConv_ThisCall, /*returnInfo*/NULL, pass, /*numparams*/2));
	}
	
	int target = params[1];
	CBaseEntity * pTarget = UTIL_GetCBaseEntity(target, true);
	if(pTarget == NULL) 
	{
		return pContext->ThrowNativeError("Invalid Stagger target entity");		
	}
	
	int source_ent = params[2];
	CBaseEntity * pSource = UTIL_GetCBaseEntity(source_ent, false);
	if(pSource == NULL) 
	{
		return pContext->ThrowNativeError("Invalid Stagger source entity");
	}
	
	cell_t * source_vector;
	pContext->LocalToPhysAddr(params[3], &source_vector);
	
	Vector vSourceVector;
	Vector *pSourceVector = NULL;
	
	if(source_vector != pContext->GetNullRef(SP_NULL_VECTOR))
	{
		vSourceVector[0] = sp_ctof(source_vector[0]);
		vSourceVector[1] = sp_ctof(source_vector[1]);
		vSourceVector[2] = sp_ctof(source_vector[2]);
		pSourceVector = &vSourceVector;
	}
	
	unsigned char vstk[sizeof(CBaseEntity *) + sizeof(CBaseEntity *) + sizeof(Vector *)];
	unsigned char *vptr = vstk;

	*(CBaseEntity **)vptr = pTarget;
	vptr += sizeof(CBaseEntity *);
	*(CBaseEntity **)vptr = pSource;
	vptr += sizeof(CBaseEntity *);
	*(Vector **)vptr = pSourceVector;

	L4D_DEBUG_LOG("Going to execute CTerrorPlayer::OnStaggered");
	pWrapper->Execute(vstk, NULL);
	L4D_DEBUG_LOG("Invoked CTerrorPlayer::OnStaggered");
	
	return 0;
}

cell_t L4D_GetMobSpawnTimerRemaining(IPluginContext *pContext, const cell_t *params)
{
	if (g_pDirector == NULL)
	{
		return pContext->ThrowNativeError("Director unsupported or not available; file a bug report");
	}

	CDirector *director = *g_pDirector;

	if (director == NULL)
	{
		return pContext->ThrowNativeError("Director not available before map is loaded");
	}
	L4D_DEBUG_LOG("Reading director mobspawntimer");
	return sp_ftoc(director->MobSpawnTimer.GetRemainingTime());
}

cell_t L4D_GetMobSpawnTimerDuration(IPluginContext *pContext, const cell_t *params)
{
	if (g_pDirector == NULL)
	{
		return pContext->ThrowNativeError("Director unsupported or not available; file a bug report");
	}

	CDirector *director = *g_pDirector;

	if (director == NULL)
	{
		return pContext->ThrowNativeError("Director not available before map is loaded");
	}
	L4D_DEBUG_LOG("Reading director mobspawntimer duration");
	return sp_ftoc(director->MobSpawnTimer.GetCountdownDuration());
}

// L4D_GetPlayerSpawnTime(client)
cell_t L4D_GetPlayerSpawnTime(IPluginContext *pContext, const cell_t *params)
{	
	static int offset;
	if(!offset && (!g_pGameConf->GetOffset("SpawnTimer", &offset) || !offset))
	{
		return pContext->ThrowNativeError("Could not read 'VersusMaxCompletionScore' offset from GameConf");
	}
	

	int player = params[1];
	CBaseEntity * pPlayer = UTIL_GetCBaseEntity(player, true);
	if(pPlayer == NULL) 
	{
		return pContext->ThrowNativeError("Invalid Player to retrieve spawn timer");		
	}
	L4D_DEBUG_LOG("Reading player %d spawn timer", params[1]);
	
	return sp_ftoc(*(float *)((char*)pPlayer+offset) - gpGlobals->curtime);
}

// ZombieManager::ReplaceTank(CTerrorPlayer*, CTerrorPlayer*);
// native L4D_ReplaceTank(tank, newtank)
cell_t L4D_ReplaceTank(IPluginContext *pContext, const cell_t *params)
{
	if (g_pZombieManager == NULL)
	{
		return pContext->ThrowNativeError("ZombieManager unsupported or not available; file a bug report");
	}

	static ICallWrapper *pWrapper = NULL;

	// ZombieManager::ReplaceTank(CTerrorPlayer*, CTerrorPlayer*)
	if (!pWrapper)
	{
		REGISTER_NATIVE_ADDR("ReplaceTank", 
			PassInfo pass[2]; \
			pass[0].flags = PASSFLAG_BYVAL; \
			pass[0].size = sizeof(CBaseEntity *); \
			pass[0].type = PassType_Basic; \
			pass[1].flags = PASSFLAG_BYVAL; \
			pass[1].size = sizeof(CBaseEntity *); \
			pass[1].type = PassType_Basic; \
			pWrapper = g_pBinTools->CreateCall(addr, CallConv_ThisCall, /*returnInfo*/NULL, pass, /*numparams*/2));
	}
	
	int tank = params[1];
	CBaseEntity * pTank = UTIL_GetCBaseEntity(tank, true);
	if(pTank == NULL) 
	{
		return pContext->ThrowNativeError("Invalid Tank client");		
	}
	
	int newtank = params[2];
	CBaseEntity * pNewtank = UTIL_GetCBaseEntity(newtank, true);
	if(pNewtank == NULL) 
	{
		return pContext->ThrowNativeError("Invalid New Tank client");
	}
	
	unsigned char vstk[sizeof(void *) + sizeof(CBaseEntity *) + sizeof(CBaseEntity *)];
	unsigned char *vptr = vstk;

	*(void **)vptr = g_pZombieManager;
	vptr += sizeof(void *);
	
	*(CBaseEntity **)vptr = pTank;
	vptr += sizeof(CBaseEntity *);
	
	*(CBaseEntity **)vptr = pNewtank;

	pWrapper->Execute(vstk, NULL);
	
	return 0;
}

// CDirectorScriptedEventManager::SendInRescueVehicle(void)
// L4D2_SendInRescueVehicle()
cell_t L4D2_SendInRescueVehicle(IPluginContext *pContext, const cell_t *params)
{
	static ICallWrapper *pWrapper = NULL;

	if (!pWrapper)
	{
		REGISTER_NATIVE_ADDR("SendInRescueVehicle", 
			pWrapper = g_pBinTools->CreateCall(addr, CallConv_ThisCall, /*returnInfo*/NULL, /*Pass*/NULL, /*numparams*/0));
	}
	
	if (g_pDirector == NULL)
	{
		return pContext->ThrowNativeError("Director unsupported or not available; file a bug report");
	}

	void *scriptedeventmanager = (*g_pDirector)->ScriptedEventManagerPtr;

	if (scriptedeventmanager == NULL)
	{
		return pContext->ThrowNativeError("DirectorScriptedEventManager pointer is NULL");
	}
	
	/* Build the vcall argument stack */
	unsigned char vstk[sizeof(void *)];
	unsigned char *vptr = vstk;

	*(void **)vptr = scriptedeventmanager;
	pWrapper->Execute(vstk, /*retbuffer*/NULL);
	
	return 0;
}

// CDirectorScriptedEventManager::ChangeFinaleStage(CDirectorScriptedEventManager::FinaleStageType,char  const*)
// L4D2_ChangeFinaleStage(finaleType, String:input)
cell_t L4D2_ChangeFinaleStage(IPluginContext *pContext, const cell_t *params)
{
	static ICallWrapper *pWrapper = NULL;

	if (!pWrapper)
	{
		REGISTER_NATIVE_ADDR("ChangeFinaleStage", 
			PassInfo pass[2]; \
			pass[0].flags = PASSFLAG_BYVAL; \
			pass[0].size = sizeof(int); \
			pass[0].type = PassType_Basic; \
			pass[1].flags = PASSFLAG_BYVAL; \
			pass[1].size = sizeof(char *); \
			pass[1].type = PassType_Basic; \
			pWrapper = g_pBinTools->CreateCall(addr, CallConv_ThisCall, /*returnInfo*/NULL, /*Pass*/pass, /*numparams*/2));
	}
	
	if (g_pDirector == NULL)
	{
		return pContext->ThrowNativeError("Director unsupported or not available; file a bug report");
	}

	void *scriptedeventmanager = (*g_pDirector)->ScriptedEventManagerPtr;

	if (scriptedeventmanager == NULL)
	{
		return pContext->ThrowNativeError("DirectorScriptedEventManager pointer is NULL");
	}
	
	/* Build the vcall argument stack */
	unsigned char vstk[sizeof(void *) + sizeof(int) + sizeof(char *)];
	unsigned char *vptr = vstk;

	*(void **)vptr = scriptedeventmanager;
	vptr += sizeof(CDirectorScriptedEventManager *);
	
	*(int *)vptr = params[1];
	vptr += sizeof(int *);

	char *arg = NULL;
	pContext->LocalToString(params[2], &arg);
	*(char **)vptr = arg;

	pWrapper->Execute(vstk, /*retbuffer*/NULL);
	
	return 0;
}

cell_t L4D2_SpawnSpecial(IPluginContext *pContext, const cell_t *params)
{
	if (g_pZombieManager == NULL)
	{
		return pContext->ThrowNativeError("ZombieManager unsupported or not available; file a bug report");
	}

	static ICallWrapper *pWrapper = NULL;

	if (!pWrapper)
	{
		PassInfo passInfo;
		passInfo.flags = PASSFLAG_BYVAL;
		passInfo.size = sizeof( CBaseEntity* );
		passInfo.type = PassType_Basic;

		REGISTER_NATIVE_ADDR("SpawnSpecial", 
			PassInfo pass[3]; \
			pass[0].flags = PASSFLAG_BYVAL; \
			pass[0].size = sizeof(ZombieClassType); \
			pass[0].type = PassType_Basic; \
			pass[1].flags = PASSFLAG_BYVAL; \
			pass[1].size = sizeof(Vector*); \
			pass[1].type = PassType_Basic; \
			pass[2].flags = PASSFLAG_BYVAL; \
			pass[2].size = sizeof(QAngle*); \
			pass[2].type = PassType_Basic; \
			pWrapper = g_pBinTools->CreateCall(addr, CallConv_ThisCall, &passInfo, pass, 3));
	}

	CBaseEntity* entity;
	
	unsigned char vstk[ sizeof(void*) + sizeof(ZombieClassType) + sizeof(Vector*) + sizeof(QAngle*) ];
	unsigned char *vptr = vstk;
	
	cell_t* source_vector;
	pContext->LocalToPhysAddr(params[2], &source_vector);

	cell_t* source_qangle;
	pContext->LocalToPhysAddr(params[3], &source_qangle);

	Vector vector;
	QAngle qangle;

	if(source_vector != pContext->GetNullRef(SP_NULL_VECTOR))
	{
		vector[0] = sp_ctof(source_vector[0]);
		vector[1] = sp_ctof(source_vector[1]);
		vector[2] = sp_ctof(source_vector[2]);
	}
	if(source_qangle != pContext->GetNullRef(SP_NULL_VECTOR))
	{
		qangle[0] = sp_ctof(source_qangle[0]);
		qangle[1] = sp_ctof(source_qangle[1]);
		qangle[2] = sp_ctof(source_qangle[2]);
	}
	
	*(void**)vptr = g_pZombieManager;
	vptr += sizeof(void*);
	
	*(cell_t*)vptr = params[1];
	vptr += sizeof(ZombieClassType);

	*(Vector**)vptr = &vector;
	vptr += sizeof(Vector*);

	*(QAngle**)vptr = &qangle;

	pWrapper->Execute( vstk, (void*) &entity );
	return gamehelpers->EntityToBCompatRef( entity );
}

cell_t L4D2_SpawnTank(IPluginContext *pContext, const cell_t *params)
{
	if (g_pZombieManager == NULL)
	{
		return pContext->ThrowNativeError("ZombieManager unsupported or not available; file a bug report");
	}

	static ICallWrapper *pWrapper = NULL;

	if (!pWrapper)
	{
		PassInfo passInfo;
		passInfo.flags = PASSFLAG_BYVAL;
		passInfo.size = sizeof( CBaseEntity* );
		passInfo.type = PassType_Basic;

		REGISTER_NATIVE_ADDR("SpawnTank", 
			PassInfo pass[2]; \
			pass[0].flags = PASSFLAG_BYVAL; \
			pass[0].size = sizeof(Vector*); \
			pass[0].type = PassType_Basic; \
			pass[1].flags = PASSFLAG_BYVAL; \
			pass[1].size = sizeof(QAngle*); \
			pass[1].type = PassType_Basic; \
			pWrapper = g_pBinTools->CreateCall(addr, CallConv_ThisCall, &passInfo, pass, 2));
	}

	CBaseEntity* entity;
	
	unsigned char vstk[ sizeof(void*) + sizeof(Vector*) + sizeof(QAngle*) ];
	unsigned char *vptr = vstk;
	
	cell_t* source_vector;
	pContext->LocalToPhysAddr(params[1], &source_vector);

	cell_t* source_qangle;
	pContext->LocalToPhysAddr(params[2], &source_qangle);

	Vector vector;
	QAngle qangle;

	if(source_vector != pContext->GetNullRef(SP_NULL_VECTOR))
	{
		vector[0] = sp_ctof(source_vector[0]);
		vector[1] = sp_ctof(source_vector[1]);
		vector[2] = sp_ctof(source_vector[2]);
	}
	if(source_qangle != pContext->GetNullRef(SP_NULL_VECTOR))
	{
		qangle[0] = sp_ctof(source_qangle[0]);
		qangle[1] = sp_ctof(source_qangle[1]);
		qangle[2] = sp_ctof(source_qangle[2]);
	}
	
	*(void**)vptr = g_pZombieManager;
	vptr += sizeof(void*);

	*(Vector**)vptr = &vector;
	vptr += sizeof(Vector*);

	*(QAngle**)vptr = &qangle;

	pWrapper->Execute( vstk, (void*) &entity );
	return gamehelpers->EntityToBCompatRef( entity );
}

cell_t L4D2_SpawnWitch(IPluginContext *pContext, const cell_t *params)
{
	if (g_pZombieManager == NULL)
	{
		return pContext->ThrowNativeError("ZombieManager unsupported or not available; file a bug report");
	}

	static ICallWrapper *pWrapper = NULL;

	if (!pWrapper)
	{
		PassInfo passInfo;
		passInfo.flags = PASSFLAG_BYVAL;
		passInfo.size = sizeof( CBaseEntity* );
		passInfo.type = PassType_Basic;

		REGISTER_NATIVE_ADDR("SpawnWitch", 
			PassInfo pass[2]; \
			pass[0].flags = PASSFLAG_BYVAL; \
			pass[0].size = sizeof(Vector*); \
			pass[0].type = PassType_Basic; \
			pass[1].flags = PASSFLAG_BYVAL; \
			pass[1].size = sizeof(QAngle*); \
			pass[1].type = PassType_Basic; \
			pWrapper = g_pBinTools->CreateCall(addr, CallConv_ThisCall, &passInfo, pass, 2));
	}

	CBaseEntity* entity;
	
	unsigned char vstk[ sizeof(void*) + sizeof(Vector*) + sizeof(QAngle*) ];
	unsigned char *vptr = vstk;
	
	cell_t* source_vector;
	pContext->LocalToPhysAddr(params[1], &source_vector);

	cell_t* source_qangle;
	pContext->LocalToPhysAddr(params[2], &source_qangle);

	Vector vector;
	QAngle qangle;

	if(source_vector != pContext->GetNullRef(SP_NULL_VECTOR))
	{
		vector[0] = sp_ctof(source_vector[0]);
		vector[1] = sp_ctof(source_vector[1]);
		vector[2] = sp_ctof(source_vector[2]);
	}
	if(source_qangle != pContext->GetNullRef(SP_NULL_VECTOR))
	{
		qangle[0] = sp_ctof(source_qangle[0]);
		qangle[1] = sp_ctof(source_qangle[1]);
		qangle[2] = sp_ctof(source_qangle[2]);
	}
	
	*(void**)vptr = g_pZombieManager;
	vptr += sizeof(void*);

	*(Vector**)vptr = &vector;
	vptr += sizeof(Vector*);

	*(QAngle**)vptr = &qangle;

	pWrapper->Execute( vstk, (void*) &entity );
	return gamehelpers->EntityToBCompatRef( entity );	
}

cell_t L4D2_SpawnWitchBride(IPluginContext *pContext, const cell_t *params)
{
	if (g_pZombieManager == NULL)
	{
		return pContext->ThrowNativeError("ZombieManager unsupported or not available; file a bug report");
	}

	static ICallWrapper *pWrapper = NULL;

	if (!pWrapper)
	{
		PassInfo passInfo;
		passInfo.flags = PASSFLAG_BYVAL;
		passInfo.size = sizeof( CBaseEntity* );
		passInfo.type = PassType_Basic;

		REGISTER_NATIVE_ADDR("SpawnWitchBride", 
			PassInfo pass[2]; \
			pass[0].flags = PASSFLAG_BYVAL; \
			pass[0].size = sizeof(Vector*); \
			pass[0].type = PassType_Basic; \
			pass[1].flags = PASSFLAG_BYVAL; \
			pass[1].size = sizeof(QAngle*); \
			pass[1].type = PassType_Basic; \
			pWrapper = g_pBinTools->CreateCall(addr, CallConv_ThisCall, &passInfo, pass, 2));
	}

	CBaseEntity* entity;
	
	unsigned char vstk[ sizeof(void*) + sizeof(Vector*) + sizeof(QAngle*) ];
	unsigned char *vptr = vstk;
	
	cell_t* source_vector;
	pContext->LocalToPhysAddr(params[1], &source_vector);

	cell_t* source_qangle;
	pContext->LocalToPhysAddr(params[2], &source_qangle);

	Vector vector;
	QAngle qangle;

	if(source_vector != pContext->GetNullRef(SP_NULL_VECTOR))
	{
		vector[0] = sp_ctof(source_vector[0]);
		vector[1] = sp_ctof(source_vector[1]);
		vector[2] = sp_ctof(source_vector[2]);
	}
	if(source_qangle != pContext->GetNullRef(SP_NULL_VECTOR))
	{
		qangle[0] = sp_ctof(source_qangle[0]);
		qangle[1] = sp_ctof(source_qangle[1]);
		qangle[2] = sp_ctof(source_qangle[2]);
	}
	
	*(void**)vptr = g_pZombieManager;
	vptr += sizeof(void*);

	*(Vector**)vptr = &vector;
	vptr += sizeof(Vector*);

	*(QAngle**)vptr = &qangle;

	pWrapper->Execute( vstk, (void*) &entity );
	return gamehelpers->EntityToBCompatRef( entity );
}

sp_nativeinfo_t g_L4DoNatives[] = 
{
	{"L4D_GetTeamScore",				L4D_GetTeamScore},
	{"L4D_GetCampaignScores",			L4D_GetCampaignScores},
	{"L4D_RestartScenarioFromVote",		L4D_RestartScenarioFromVote},
	{"L4D_LobbyUnreserve",				L4D_LobbyUnreserve},
	{"L4D_LobbyIsReserved",				L4D_LobbyIsReserved},
	{"L4D_ScavengeBeginRoundSetupTime", L4D_ScavengeBeginRoundSetupTime},
	{"L4D_GetVersusMaxCompletionScore",	L4D_GetVersusMaxCompletionScore},
	{"L4D_SetVersusMaxCompletionScore",	L4D_SetVersusMaxCompletionScore},
	{"L4D_IsMissionFinalMap",			L4D_IsMissionFinalMap},
	{"L4D_ResetMobTimer",				L4D_ResetMobTimer},
	{"L4D_NotifyNetworkStateChanged",	L4D_NotifyNetworkStateChanged},
	{"L4D_StaggerPlayer",				L4D_StaggerPlayer},
	{"L4D_GetMobSpawnTimerRemaining",	L4D_GetMobSpawnTimerRemaining},
	{"L4D_GetMobSpawnTimerDuration",	L4D_GetMobSpawnTimerDuration},
	{"L4D_GetPlayerSpawnTime",  		L4D_GetPlayerSpawnTime},
	{"L4D_ReplaceTank",  				L4D_ReplaceTank},
	{"L4D2_SendInRescueVehicle",  		L4D2_SendInRescueVehicle},
	{"L4D2_ChangeFinaleStage",  		L4D2_ChangeFinaleStage},
	{"L4D2_SpawnSpecial",				L4D2_SpawnSpecial},
	{"L4D2_SpawnTank",					L4D2_SpawnTank},
	{"L4D2_SpawnWitch",					L4D2_SpawnWitch},
	{"L4D2_SpawnWitchBride",  			L4D2_SpawnWitchBride},
	{NULL,							NULL}
};
