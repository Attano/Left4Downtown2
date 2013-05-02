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

#ifndef _INCLUDE_SOURCEMOD_DETOUR_HAS_CONFIGURABLE_DIFFICULTY_H_
#define _INCLUDE_SOURCEMOD_DETOUR_HAS_CONFIGURABLE_DIFFICULTY_H_

#include "detour_template.h"

namespace Detours {
	
class HasConfigurableDifficulty;

typedef int (HasConfigurableDifficulty::*HasConfigurableDifficultyFunc)();

class HasConfigurableDifficulty : public DetourTemplate<HasConfigurableDifficultyFunc, HasConfigurableDifficulty>
{
private: //note: implementation of DetourTemplate abstracts

	int OnHasConfigurableDifficulty();

	// get the signature name (i.e. "HasConfigurableDifficulty") from the game conf
	virtual const char * GetSignatureName()
	{
#ifdef PLATFORM_LINUX
		return "HasConfigurableDifficulty";
	}
#elif defined PLATFORM_WINDOWS
		return NULL;
	}
	
	virtual unsigned char *GetSignatureAddress()
	{
		unsigned char* signature;
		if (!gameconf->GetMemSig("DifficultyChanged", (void**)&signature) || !signature) 
		{ 
			g_pSM->LogError(myself, "Detour -- Could not find DifficultyChanged signature");
			return NULL;
		}
		int offset;
		if (!gameconf->GetOffset("HasConfigurableDifficultySetting", &offset) || !offset)
		{
			g_pSM->LogError(myself, "Detour -- Could not find HasConfigurableDifficultySetting offset");
			return NULL;
		}
		L4D_DEBUG_LOG("Calculated HasConfigurableDifficulty address %p + offset %d = %p", signature, offset, (int)signature + offset);
		int functionoffset = *(int *)((int)signature + offset);
		L4D_DEBUG_LOG("HasConfigurableDifficulty Call offset %p + %d", (int)signature + offset + 4, functionoffset);
		signature = (unsigned char *)((int)signature + offset + 4 + functionoffset);
		L4D_DEBUG_LOG("About to read from calculated HasConfigurableDifficulty address %p", signature);
		// first byte should be a call
		if(*signature != 0xE8)
		{
			g_pSM->LogError(myself, "Byte compare failed for HasConfigurableDifficulty function at %p", signature);
			return NULL;
		}
		return signature;
	}
#endif

	//notify our patch system which function should be used as the detour
	virtual HasConfigurableDifficultyFunc GetDetour()
	{
		return &HasConfigurableDifficulty::OnHasConfigurableDifficulty;
	}
};

};
#endif
