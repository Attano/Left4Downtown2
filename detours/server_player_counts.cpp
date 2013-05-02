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

#include "server_player_counts.h"
#include "player_slots.h"
#include "extension.h"

namespace Detours
{
	/*
	On Windows overwriting maxPlayers makes the server browser display that value
	On Linux overwriting maxPlayers only makes 'status' show that value as the max
	*/
	int ServerPlayerCounts::OnGetMasterServerPlayerCounts(int* currentPlayers, int* maxPlayers, int* unknownArg)
	{
		int arg1, arg2, arg3;

		int results = (this->*(GetTrampoline()))(&arg1, &arg2, &arg3);

		//spams log every 2 seconds
		//L4D_DEBUG_LOG("CServer::GetMasterServerPlayerCounts returned %d, args(%d, %d, %d) / MaxSlots = %d", results, arg1, arg2, arg3, ::PlayerSlots::MaxSlots);

		*currentPlayers = arg1;
		*maxPlayers = arg2;
		*unknownArg = arg3; //always 0 in brief testing

		if(::PlayerSlots::MaxSlots != -1)
		{
			*maxPlayers = ::PlayerSlots::MaxSlots;
		}

		return results;
	}
};
