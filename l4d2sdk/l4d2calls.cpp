/**
 * vim: set ts=4 :
 * =============================================================================
 * Left 4 Downtown 2 SourceMod Extension
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

 
  // Reproduced or wrapped valve calls
 
#include "l4d2calls.h" 
#include "extension.h"

// Get The Current Map's Mission KeyValues
KeyValues *CTerrorGameRules__GetMissionCurrentMap(KeyValues **kv)
{
	static ICallWrapper *pWrapper = NULL;
	if (!pWrapper)
	{
		PassInfo ret;
		ret.flags = PASSFLAG_BYVAL;
		ret.size = sizeof(KeyValues *);
		ret.type = PassType_Basic;

		void *addr;
		if (!g_pGameConf->GetMemSig("GetMissionCurrentMap", &addr) || !addr)
		{
			g_pSM->LogError(myself, "Could not find 'GetMissionCurrentMap' signature");
			return NULL;
		}
		
		PassInfo pass[1];
		pass[0].flags = PASSFLAG_BYVAL;
		pass[0].size = sizeof(KeyValues **);
		pass[0].type = PassType_Basic;
		
		pWrapper = g_pBinTools->CreateCall(addr, CallConv_Cdecl, &ret, pass, /*numparams*/1);
	}
	
	/* Build the vcall argument stack */
	unsigned char vstk[sizeof(KeyValues **)];
	unsigned char *vptr = vstk;

	*(KeyValues***)vptr = kv;

	KeyValues* ret;
	
	L4D_DEBUG_LOG("Going to execute CTerrorGameRules::GetMissionCurrentMap()");
	pWrapper->Execute(vstk, &ret);
	
	return ret;
}


