/**
 * vim: set ts=4 :
 * =============================================================================
 * Left 4 Downtown 2 SourceMod Extension
 * Copyright (C) 2010 Michael "ProdigySim" Busby.
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

enum L4D2CountdownTimers 
{
	L4D2CT_MobSpawnTimer,
	L4D2CT_SmokerSpawnTimer,
	L4D2CT_BoomerSpawnTimer,
	L4D2CT_HunterSpawnTimer,
	L4D2CT_SpitterSpawnTimer,
	L4D2CT_JockeySpawnTimer,
	L4D2CT_ChargerSpawnTimer,
	L4D2CT_VersusStartTimer,
	L4D2CT_UpdateMarkersTimer
};

enum L4D2IntervalTimers
{
	L4D2IT_SmokerDeathTimer,
	L4D2IT_BoomerDeathTimer,
	L4D2IT_HunterDeathTimer,
	L4D2IT_SpitterDeathTimer,
	L4D2IT_JockeyDeathTimer,
	L4D2IT_ChargerDeathTimer
};

CountdownTimer * IdToCTimer(int id)
{
	if (g_pDirector == NULL)
	{
		return NULL;
	}
	CDirector * director = *g_pDirector;
	if (director == NULL)
	{
		return NULL;
	}
	
	switch (id)
	{
		case L4D2CT_MobSpawnTimer:
			return &(director->MobSpawnTimer);
		case L4D2CT_SmokerSpawnTimer:
			return &(director->SmokerSpawnTimer);
		case L4D2CT_BoomerSpawnTimer:
			return &(director->BoomerSpawnTimer);
		case L4D2CT_HunterSpawnTimer:
			return &(director->HunterSpawnTimer);
		case L4D2CT_SpitterSpawnTimer:
			return &(director->SpitterSpawnTimer);
		case L4D2CT_JockeySpawnTimer:
			return &(director->JockeySpawnTimer);
		case L4D2CT_ChargerSpawnTimer:
			return &(director->ChargerSpawnTimer);
		case L4D2CT_VersusStartTimer:
			if(director->VersusModePtr == NULL)
				return NULL;
			return &((director->VersusModePtr)->VersusStartTimer);
		case L4D2CT_UpdateMarkersTimer:
			if(director->VersusModePtr == NULL)
				return NULL;
			return &((director->VersusModePtr)->UpdateMarkersTimer);
		default:
			return NULL;
	}
}

IntervalTimer * IdToITimer(int id)
{
	if (g_pDirector == NULL)
	{
		return NULL;
	}
	CDirector *director = *g_pDirector;
	if (director == NULL)
	{
		return NULL;
	}
	
	switch(id)
	{
		case L4D2IT_SmokerDeathTimer:
			return &(director->m_ClassDeathTimers[ZombieClassType_Smoker]);
		case L4D2IT_BoomerDeathTimer:
			return &(director->m_ClassDeathTimers[ZombieClassType_Boomer]);
		case L4D2IT_HunterDeathTimer:
			return &(director->m_ClassDeathTimers[ZombieClassType_Hunter]);
		case L4D2IT_SpitterDeathTimer:
			return &(director->m_ClassDeathTimers[ZombieClassType_Spitter]);
		case L4D2IT_JockeyDeathTimer:
			return &(director->m_ClassDeathTimers[ZombieClassType_Jockey]);
		case L4D2IT_ChargerDeathTimer:
			return &(director->m_ClassDeathTimers[ZombieClassType_Charger]);
		default:
			return NULL;
	}
}

/*************************************
	CountdownTimer Natives 
	***********************************/
// native L4D2_CTimerReset(L4D2CountdownTimer:timer);
cell_t L4D2_CTimerReset(IPluginContext *pContext, const cell_t *params)
{
	CountdownTimer * myTimer = IdToCTimer(params[1]);
	if(myTimer == NULL)
	{
		return pContext->ThrowNativeError("Invalid timer index or timer unavailable");
	}
	myTimer->Reset();
	return 0;
}

// native L4D2_CTimerStart(L4D2CountdownTimer:timer, Float:duration);
cell_t L4D2_CTimerStart(IPluginContext *pContext, const cell_t *params)
{
	CountdownTimer * myTimer = IdToCTimer(params[1]);
	if(myTimer == NULL)
	{
		return pContext->ThrowNativeError("Invalid timer index or timer unavailable");
	}
	myTimer->Start(sp_ctof(params[2]));
	return 0;
}

// native L4D2_CTimerInvalidate(L4D2CountdownTimer:timer);
cell_t L4D2_CTimerInvalidate(IPluginContext *pContext, const cell_t *params)
{
	CountdownTimer * myTimer = IdToCTimer(params[1]);
	if(myTimer == NULL)
	{
		return pContext->ThrowNativeError("Invalid timer index or timer unavailable");
	}
	myTimer->Invalidate();
	return 0;
}

// native bool:L4D2_CTimerHasStarted(L4D2CountdownTimer:timer);
cell_t L4D2_CTimerHasStarted(IPluginContext *pContext, const cell_t *params)
{
	CountdownTimer * myTimer = IdToCTimer(params[1]);
	if(myTimer == NULL)
	{
		return pContext->ThrowNativeError("Invalid timer index or timer unavailable");
	}
	return myTimer->HasStarted();
}

// native bool:L4D2_CTimerIsElapsed(L4D2CountdownTimer:timer);
cell_t L4D2_CTimerIsElapsed(IPluginContext *pContext, const cell_t *params)
{
	CountdownTimer * myTimer = IdToCTimer(params[1]);
	if(myTimer == NULL)
	{
		return pContext->ThrowNativeError("Invalid timer index or timer unavailable");
	}
	return myTimer->IsElapsed();
}

// native Float:L4D2_CTimerGetElapsedTime(L4D2CountdownTimer:timer);
cell_t L4D2_CTimerGetElapsedTime(IPluginContext *pContext, const cell_t *params)
{
	CountdownTimer * myTimer = IdToCTimer(params[1]);
	if(myTimer == NULL)
	{
		return pContext->ThrowNativeError("Invalid timer index or timer unavailable");
	}
	return sp_ftoc(myTimer->GetElapsedTime());
}

// native Float:L4D2_CTimerGetRemainingTime(L4D2CountdownTimer:timer);
cell_t L4D2_CTimerGetRemainingTime(IPluginContext *pContext, const cell_t *params)
{
	CountdownTimer * myTimer = IdToCTimer(params[1]);
	if(myTimer == NULL)
	{
		return pContext->ThrowNativeError("Invalid timer index or timer unavailable");
	}
	return sp_ftoc(myTimer->GetRemainingTime());
}

// native Float:L4D2_CTimerGetCountdownDuration(L4D2CountdownTimer:timer);
cell_t L4D2_CTimerGetCountdownDuration(IPluginContext *pContext, const cell_t *params)
{
	CountdownTimer * myTimer = IdToCTimer(params[1]);
	if(myTimer == NULL)
	{
		return pContext->ThrowNativeError("Invalid timer index or timer unavailable");
	}
	return sp_ftoc(myTimer->GetCountdownDuration());
}

/*************************************
	IntervalTimer Natives 
	***********************************/

// native L4D2_ITimerStart(L4D2IntervalTimer:timer);
cell_t L4D2_ITimerStart(IPluginContext *pContext, const cell_t *params)
{
	IntervalTimer * myTimer = IdToITimer(params[1]);
	if(myTimer == NULL)
	{
		return pContext->ThrowNativeError("Invalid timer index or timer unavailable");
	}
	myTimer->Start();
	return 0;
}
	
// native L4D2_ITimerInvalidate(L4D2IntervalTimer:timer);
cell_t L4D2_ITimerInvalidate(IPluginContext *pContext, const cell_t *params)
{
	IntervalTimer * myTimer = IdToITimer(params[1]);
	if(myTimer == NULL)
	{
		return pContext->ThrowNativeError("Invalid timer index or timer unavailable");
	}
	myTimer->Invalidate();
	return 0;
}
	
// native bool:L4D2_ITimerHasStarted(L4D2IntervalTimer:timer);
cell_t L4D2_ITimerHasStarted(IPluginContext *pContext, const cell_t *params)
{
	IntervalTimer * myTimer = IdToITimer(params[1]);
	if(myTimer == NULL)
	{
		return pContext->ThrowNativeError("Invalid timer index or timer unavailable");
	}
	return myTimer->HasStarted();
}
	
// native Float:L4D2_ITimerGetElapsedTime(L4D2IntervalTimer:timer);
cell_t L4D2_ITimerGetElapsedTime(IPluginContext *pContext, const cell_t *params)
{
	IntervalTimer * myTimer = IdToITimer(params[1]);
	if(myTimer == NULL)
	{
		return pContext->ThrowNativeError("Invalid timer index or timer unavailable");
	}
	return sp_ftoc(myTimer->GetElapsedTime());
}

sp_nativeinfo_t  g_L4DoTimerNatives[] = 
{
	{"L4D2_CTimerReset",  				L4D2_CTimerReset},
	{"L4D2_CTimerStart",  				L4D2_CTimerStart},
	{"L4D2_CTimerInvalidate",  			L4D2_CTimerInvalidate},
	{"L4D2_CTimerHasStarted",  			L4D2_CTimerHasStarted},
	{"L4D2_CTimerIsElapsed",  			L4D2_CTimerIsElapsed},
	{"L4D2_CTimerGetElapsedTime",  		L4D2_CTimerGetElapsedTime},
	{"L4D2_CTimerGetRemainingTime",  	L4D2_CTimerGetRemainingTime},
	{"L4D2_CTimerGetCountdownDuration", L4D2_CTimerGetCountdownDuration},
	{"L4D2_ITimerStart",  				L4D2_ITimerStart},
	{"L4D2_ITimerInvalidate",			L4D2_ITimerInvalidate},
	{"L4D2_ITimerHasStarted",			L4D2_ITimerHasStarted},
	{"L4D2_ITimerGetElapsedTime",		L4D2_ITimerGetElapsedTime},
	{NULL,										NULL}
};
