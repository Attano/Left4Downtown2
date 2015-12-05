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

#include "clear_team_scores.h"
#include "extension.h"

namespace Detours
{

/* CTerrorGameRules::ClearTeamScores in L4D2 has a bool parameter -XBetaAlpha		*/

	void ClearTeamScores::OnClearTeamScores(CLEAR_TEAM_SCORES_ARG)
	{
		bool b = newCampaign;

		L4D_DEBUG_LOG("CTerrorGameRules::ClearTeamScores has been called, bool=%d", b);

		cell_t result = Pl_Continue;
		if(g_pFwdOnClearTeamScores)
		{
			L4D_DEBUG_LOG("L4D_OnClearTeamScores forward has been sent out");
			g_pFwdOnClearTeamScores->PushCell(static_cast<cell_t>(b));
			g_pFwdOnClearTeamScores->Execute(&result);
		}

		if(result == Pl_Handled)
		{
			L4D_DEBUG_LOG("CTerrorGameRules::ClearTeamScores will be skipped");
			return;
		}
		else
		{
			(this->*(GetTrampoline()))(b);
			return;
		}
	}
};
