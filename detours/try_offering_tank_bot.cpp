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

#include "try_offering_tank_bot.h"
#include "extension.h"

namespace Detours
{
	void TryOfferingTankBot::OnTryOfferingTankBot(CBaseEntity* tank, bool enterStasis)
	{
		L4D_DEBUG_LOG("CTerrorPlayer::TryOfferingTankBot has been called");

		cell_t result = Pl_Continue;
		if(g_pFwdOnTryOfferingTankBot)
		{
			cell_t tankindex = tank ? IndexOfEdict(gameents->BaseEntityToEdict(tank)) : 0;
			cell_t cellEnterStasis = static_cast<bool>(enterStasis);
			
			L4D_DEBUG_LOG("L4D_OnTryOfferingTankBot(tank %d, enterStasis %d) forward has been sent out", tank, enterStasis);
			g_pFwdOnTryOfferingTankBot->PushCell(tankindex);
			g_pFwdOnTryOfferingTankBot->PushCellByRef(&cellEnterStasis);
			L4D_DEBUG_LOG("L4D_OnTryOfferingTankBot() forward has been sent out");
			g_pFwdOnTryOfferingTankBot->Execute(&result);
			enterStasis = cellEnterStasis != 0;
		}

		if(result == Pl_Handled)
		{
			L4D_DEBUG_LOG("CDirector::TryOfferingTankBot will be skipped");
			return;
		}
		else
		{
			L4D_DEBUG_LOG("CTerrorGameRules::SetCampaignScores will be invoked with enterStasis=%d", enterStasis);
			(this->*(GetTrampoline()))(tank, enterStasis);
			return;
		}
	}
};
