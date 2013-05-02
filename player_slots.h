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

#ifndef _INCLUDE_SOURCEMOD_PLAYER_SLOTS_H_
#define _INCLUDE_SOURCEMOD_PLAYER_SLOTS_H_

#define RESTRICT_MAX_PLAYERS_BY_COMMAND_LINE 1
#define DEFAULT_MAX_PLAYERS 8

#include "codepatch/icodepatch.h"

class PlayerSlots
{
public:

	/* 
		patch server/engine to allow more than 8 player slots
	*/
	static void Patch();

	/*
		remove the previous patches and restore the binaries back to normal 
	*/
	static void Unpatch();

	static void PatchSlotCheckOnly();
	static void UnpatchSlotCheckOnly();

	static void PatchGetMaxHumanPlayers();
	static void UnpatchGetMaxHumanPlayers();

/* - XBetaAlpha - Future implementation

        static void PatchGetTotalNumPlayersSupported();
        static void UnpatchGetTotalNumPlayersSupported();

* EOP */

	static void OnMaxSlotsChanged(int max_slots);

	static int MaxClients; /* as defined by GetMaxClients() */
	static int MaxSlots; /* the actual maxslots we will use in patch */
	static int MaxPlayers; /* command line +-maxplayers setting */
};


void OnMaxPlayersChanged( IConVar *var, const char *pOldValue, float flOldValue );

#endif
