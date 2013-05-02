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

cell_t L4D2_GetTankCount(IPluginContext *pContext, const cell_t *params)
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
	return director->m_iTankCount;
}

cell_t L4D2_GetVersusCampaignScores(IPluginContext *pContext, const cell_t *params)
{
	if (g_pDirector == NULL)
	{
		return pContext->ThrowNativeError("Director unsupported or not available; file a bug report");
	}
	if (*g_pDirector == NULL)
	{
		return pContext->ThrowNativeError("Director not available before map is loaded");
	}
	CDirectorVersusMode * director=(*g_pDirector)->VersusModePtr;
	if(director == NULL)
	{
		return pContext->ThrowNativeError("DirectorVersusMode not available--is this versus mode?");
	}
	cell_t * scores;
	pContext->LocalToPhysAddr(params[1], &scores);
	scores[0]=director->m_iCampaignScores[0];
	scores[1]=director->m_iCampaignScores[1];
	return 0;
}

cell_t L4D2_SetVersusCampaignScores(IPluginContext *pContext, const cell_t *params)
{
	if (g_pDirector == NULL)
	{
		return pContext->ThrowNativeError("Director unsupported or not available; file a bug report");
	}
	if (*g_pDirector == NULL)
	{
		return pContext->ThrowNativeError("Director not available before map is loaded");
	}
	CDirectorVersusMode * director=(*g_pDirector)->VersusModePtr;
	if(director == NULL)
	{
		return pContext->ThrowNativeError("DirectorVersusMode not available--is this versus mode?");
	}
	cell_t * scores;
	pContext->LocalToPhysAddr(params[1], &scores);
	director->m_iCampaignScores[0]=scores[0];
	director->m_iCampaignScores[1]=scores[1];
	return 0;
}

cell_t L4D2_GetVersusTankFlowPercent(IPluginContext *pContext, const cell_t *params)
{
	if (g_pDirector == NULL)
	{
		return pContext->ThrowNativeError("Director unsupported or not available; file a bug report");
	}
	if (*g_pDirector == NULL)
	{
		return pContext->ThrowNativeError("Director not available before map is loaded");
	}
	CDirectorVersusMode * director=(*g_pDirector)->VersusModePtr;
	if(director == NULL)
	{
		return pContext->ThrowNativeError("DirectorVersusMode not available--is this versus mode?");
	}
	cell_t * cell_flows;
	pContext->LocalToPhysAddr(params[1], &cell_flows);
	cell_flows[0]=sp_ftoc(director->m_fTankSpawnFlowPercent[0]);
	cell_flows[1]=sp_ftoc(director->m_fTankSpawnFlowPercent[1]);
	return 0;
}

cell_t L4D2_SetVersusTankFlowPercent(IPluginContext *pContext, const cell_t *params)
{
	if (g_pDirector == NULL)
	{
		return pContext->ThrowNativeError("Director unsupported or not available; file a bug report");
	}
	if (*g_pDirector == NULL)
	{
		return pContext->ThrowNativeError("Director not available before map is loaded");
	}
	CDirectorVersusMode * director=(*g_pDirector)->VersusModePtr;
	if(director == NULL)
	{
		return pContext->ThrowNativeError("DirectorVersusMode not available--is this versus mode?");
	}
	cell_t * cell_flows;
	pContext->LocalToPhysAddr(params[1], &cell_flows);
	director->m_fTankSpawnFlowPercent[0]=sp_ctof(cell_flows[0]);
	director->m_fTankSpawnFlowPercent[1]=sp_ctof(cell_flows[1]);
	return 0;
}

sp_nativeinfo_t  g_L4DoDirectorNatives[] = 
{
	{"L4D2_GetTankCount",				L4D2_GetTankCount},
	{"L4D2_GetVersusCampaignScores",	L4D2_GetVersusCampaignScores},
	{"L4D2_SetVersusCampaignScores",	L4D2_SetVersusCampaignScores},
	{"L4D2_GetVersusTankFlowPercent",	L4D2_GetVersusTankFlowPercent},
	{"L4D2_SetVersusTankFlowPercent",	L4D2_SetVersusTankFlowPercent},
	{NULL,										NULL}
};
