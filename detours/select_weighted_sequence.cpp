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

#include "select_weighted_sequence.h"
#include "extension.h"

#define ACT_HULK_THROW 761
#define ACT_TANK_OVERHEAD_THROW 762
#define ACT_HULK_ATTACK_LOW 763
#define ACT_TERROR_ATTACK_MOVING 790

namespace Detours
{
	int SelectWeightedSequence::OnSelectWeightedSequence(int Activity)
	{
		//L4D_DEBUG_LOG("CBaseAnimating::SelectWeightedSequence(%d) has been called", Activity);
		cell_t result = Pl_Continue;
		int actualSequence;
		actualSequence = (this->*(GetTrampoline()))(Activity);

		if ((Activity != ACT_HULK_THROW) && (Activity != ACT_TANK_OVERHEAD_THROW)
			&& (Activity != ACT_HULK_ATTACK_LOW) && (Activity != ACT_TERROR_ATTACK_MOVING))
				return actualSequence;

		int overrideSequence = actualSequence;
		if (g_pFwdOnSelectTankAttack)
		{
			edict_t *pEntity = gameents->BaseEntityToEdict(reinterpret_cast<CBaseEntity*>(this));
			int client = IndexOfEdict(pEntity);
			L4D_DEBUG_LOG("L4D2_OnSelectTankAttack(client %d, sequence %d) forward has been sent out", client, overrideSequence);
			g_pFwdOnSelectTankAttack->PushCell(client);
			g_pFwdOnSelectTankAttack->PushCellByRef(&overrideSequence);
			g_pFwdOnSelectTankAttack->Execute(&result);
		}

		if(result == Pl_Handled)
		{
			L4D_DEBUG_LOG("CBaseAnimating::SelectWeightedSequence() return value overriden with %d", overrideSequence);
			return overrideSequence;
		}
		else
		{
			return actualSequence;
		}
	}
};
