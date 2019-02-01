/**
 * vim: set ts=4 :
 * =============================================================================
 * Left 4 Downtown SourceMod Extension
 * Copyright (C) 2010 Michael "ProdigySim" Busby
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

#include "choose_victim.h"
#include "extension.h"

namespace Detours
{
	CBaseEntity* BossZombiePlayerBotChooseVictim::BossZombiePlayerBotChooseVictimActivate(CBaseEntity* pCurTarget, int i1, CBaseEntity* pEnt)
	{
		cell_t result = Pl_Continue;
		int curTarget = gamehelpers->EntityToBCompatRef(pCurTarget);
		if(g_pFwdOnChooseVictim)
		{
			edict_t *pZombieBoss = gameents->BaseEntityToEdict(reinterpret_cast<CBaseEntity*>(this));
			int specialInfected = IndexOfEdict(pZombieBoss);
			
			L4D_DEBUG_LOG("L4D2_ChooseVictim(%d, %d) forward has been sent out", specialInfected, gamehelpers->EntityToBCompatRef(pCurTarget));

			g_pFwdOnChooseVictim->PushCell(specialInfected);
			g_pFwdOnChooseVictim->PushCellByRef(&curTarget);
			g_pFwdOnChooseVictim->Execute(&result);
		}
		
		if(result == Pl_Handled)
		{
			if(curTarget <= 0)
				return (this->*(GetTrampoline()))(pCurTarget, i1, pEnt);
			else
			{
				CBaseEntity *pNewTarget = gamehelpers->ReferenceToEntity(curTarget);
				return pNewTarget;
			}
		}
		else
		{
			
			return (this->*(GetTrampoline()))(pCurTarget, i1, pEnt);
		}
	}
};
