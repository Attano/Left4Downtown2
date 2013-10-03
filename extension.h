/**
 * vim: set ts=4 :
 * =============================================================================
 * Left 4 Downtown SourceMod Extension
 * Copyright (C) 2009 Igor "Downtown1" Smirnov.
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

#ifndef _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_
#define _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_

#ifdef _DEBUG
#define L4D_DEBUG_LOG(...) g_pSM->LogMessage(myself, __VA_ARGS__)
#else
#define L4D_DEBUG_LOG(...)
#endif


/**
 * @file extension.h
 * @brief Sample extension code header.
 */

#include "smsdk_ext.h"
#include <IBinTools.h>
#include <iserver.h>
#include <convar.h>
#include <icvar.h>
#include <icommandline.h>


/**
 * @brief Sample implementation of the SDK Extension.
 * Note: Uncomment one of the pre-defined virtual functions in order to use it.
 */
class Left4Downtown :
	public SDKExtension,
	public IClientListener,
	public ICommandTargetProcessor
{
public:
	/**
	 * @brief This is called after the initial loading sequence has been processed.
	 *
	 * @param error		Error message buffer.
	 * @param maxlength	Size of error message buffer.
	 * @param late		Whether or not the module was loaded after map load.
	 * @return			True to succeed loading, false to fail.
	 */
	virtual bool SDK_OnLoad(char *error, size_t maxlength, bool late);
	
	/**
	 * @brief This is called right before the extension is unloaded.
	 */
	virtual void SDK_OnUnload();

	/**
	 * @brief This is called once all known extensions have been loaded.
	 * Note: It is is a good idea to add natives here, if any are provided.
	 */
	virtual void SDK_OnAllLoaded();

	/**
	 * @brief Called when the pause state is changed.
	 */
	//virtual void SDK_OnPauseChange(bool paused);

	/**
	 * @brief this is called when Core wants to know if your extension is working.
	 *
	 * @param error		Error message buffer.
	 * @param maxlength	Size of error message buffer.
	 * @return			True if working, false otherwise.
	 */
	//virtual bool QueryRunning(char *error, size_t maxlength);
public:
#if defined SMEXT_CONF_METAMOD
	/**
	 * @brief Called when Metamod is attached, before the extension version is called.
	 *
	 * @param error			Error buffer.
	 * @param maxlength		Maximum size of error buffer.
	 * @param late			Whether or not Metamod considers this a late load.
	 * @return				True to succeed, false to fail.
	 */
	virtual bool SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlength, bool late);

	/**
	 * @brief Called when Metamod is detaching, after the extension version is called.
	 * NOTE: By default this is blocked unless sent from SourceMod.
	 *
	 * @param error			Error buffer.
	 * @param maxlength		Maximum size of error buffer.
	 * @return				True to succeed, false to fail.
	 */
	//virtual bool SDK_OnMetamodUnload(char *error, size_t maxlength);

	/**
	 * @brief Called when Metamod's pause state is changing.
	 * NOTE: By default this is blocked unless sent from SourceMod.
	 *
	 * @param paused		Pause state being set.
	 * @param error			Error buffer.
	 * @param maxlength		Maximum size of error buffer.
	 * @return				True to succeed, false to fail.
	 */
	//virtual bool SDK_OnMetamodPauseChange(bool paused, char *error, size_t maxlength);
#endif
#ifdef USE_PLAYERSLOTS_PATCHES
	/**
	 * @brief Called when the server is activated.
	 */
	virtual void OnServerActivated(int max_clients);
#endif
public: //ICommandTargetProcessor
	bool ProcessCommandTarget(cmd_target_info_t *info);
};

extern IForward *g_pFwdOnSpawnSpecial;
extern IForward *g_pFwdOnSpawnTank;
extern IForward *g_pFwdOnSpawnWitch;
extern IForward *g_pFwdOnSpawnWitchBride;
extern IForward *g_pFwdOnClearTeamScores;
extern IForward *g_pFwdOnSetCampaignScores;
extern IForward *g_pFwdOnFirstSurvivorLeftSafeArea;
extern IForward *g_pFwdOnGetScriptValueInt;
extern IForward *g_pFwdOnGetScriptValueFloat;
extern IForward *g_pFwdOnGetScriptValueString;
extern IForward *g_pFwdOnTryOfferingTankBot;
extern IForward *g_pFwdOnMobRushStart;
extern IForward *g_pFwdOnSpawnITMob;
extern IForward *g_pFwdOnSpawnMob;
extern IForward *g_pFwdOnShovedBySurvivor;
extern IForward *g_pFwdOnGetCrouchTopSpeed;
extern IForward *g_pFwdOnGetRunTopSpeed;
extern IForward *g_pFwdOnGetWalkTopSpeed;
extern IForward *g_pFwdOnHasConfigurableDifficulty;
extern IForward *g_pFwdOnGetSurvivorSet;
extern IForward *g_pFwdOnFastGetSurvivorSet;
extern IForward *g_pFwdOnGetMissionVersusBossSpawning;
extern IForward *g_pFwdOnCThrowActivate;
extern IForward *g_pFwdOnStartMeleeSwing;
extern IForward *g_pFwdOnUseHealingItems;
extern IForward *g_pFwdOnFindScavengeItem;
extern IForward *g_pFwdOnSendInRescueVehicle;
extern IForward *g_pFwdOnChangeFinaleStage;
extern IForward *g_pFwdOnEndVersusModeRound;
extern IForward *g_pFwdOnEndVersusModeRound_Post;
extern IForward *g_pFwdOnSelectTankAttack;
extern IForward *g_pFwdOnRevived;
extern IForward *g_pFwdOnWaterMove;
extern IForward *g_pFwdOnPlayerStagger;

extern bool g_bRoundEnd;

extern IBinTools *g_pBinTools;
extern IServer *g_pServer; //pointer to CBaseServer
extern IGameConfig *g_pGameConf;
extern IGameConfig *g_pGameConfSDKTools;

/* Interfaces from engine or gamedll */
extern IServerGameEnts *gameents;
extern ICvar *icvar;
extern IServer *iserver;
extern CGlobalVars *gpGlobals;
/* Interfaces from SourceMod */

#include "compat_wrappers.h"

#endif // _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_
