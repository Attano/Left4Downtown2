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

#include "get_mission_versus_boss_spawning.h"
#include "extension.h"

namespace Detours
{
	void GetMissionVersusBossSpawning::OnGetMissionVersusBossSpawning(float &spawn_pos_min, float &spawn_pos_max, float &tank_chance, float &witch_chance)
	{
		L4D_DEBUG_LOG("CDirectorVersusMode::GetMissionVersusBossSpawning has been called, %f %f %f %f", spawn_pos_min, spawn_pos_max, tank_chance, witch_chance);
		
		float ov_spawn_pos_min = spawn_pos_min;
	    float ov_spawn_pos_max = spawn_pos_max;
	    float ov_tank_chance = tank_chance;
	    float ov_witch_chance = witch_chance;

		cell_t result = Pl_Continue;
		if(g_pFwdOnGetMissionVersusBossSpawning)
		{
			L4D_DEBUG_LOG("L4D_OnGetMissionVersusBossSpawning forward has been sent out");
			g_pFwdOnGetMissionVersusBossSpawning->PushFloatByRef(&ov_spawn_pos_min);
			g_pFwdOnGetMissionVersusBossSpawning->PushFloatByRef(&ov_spawn_pos_max);
			g_pFwdOnGetMissionVersusBossSpawning->PushFloatByRef(&ov_tank_chance);
			g_pFwdOnGetMissionVersusBossSpawning->PushFloatByRef(&ov_witch_chance);
			g_pFwdOnGetMissionVersusBossSpawning->Execute(&result);
		}
		switch(result)
		{
			case Pl_Changed:
				spawn_pos_min = ov_spawn_pos_min;
				spawn_pos_max = ov_spawn_pos_max;
				tank_chance = ov_tank_chance;
				witch_chance = ov_witch_chance;
			break;
			case Pl_Handled:
			break;
			case Pl_Continue:
			default:
			(this->*(GetTrampoline()))(spawn_pos_min, spawn_pos_max, tank_chance, witch_chance);
		}

	   
		return;
	}
};
