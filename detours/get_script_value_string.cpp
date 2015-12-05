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

#include "get_script_value_string.h"
#include "extension.h"

namespace Detours
{
	void *GetScriptValueString::OnGetScriptValueString(const char *key, const char *defaultValue, char *retValue, int sizeof_retValue)
	{
		L4D_DEBUG_LOG("CDirector::GetScriptValue(%s,%s,%s,%d) is about to be called", key, defaultValue, retValue, sizeof_retValue);
		
		cell_t result = Pl_Continue;

		(this->*(GetTrampoline()))(key, defaultValue, retValue, sizeof_retValue);

		L4D_DEBUG_LOG("CDirector::GetScriptValue() passed string result: %s", retValue);
		
		if(g_pFwdOnGetScriptValueString)
		{
			//L4D_DEBUG_LOG("L4D_OnGetScriptValueString() forward has been sent out");
			g_pFwdOnGetScriptValueString->PushString(key);
			defaultValue == NULL ? g_pFwdOnGetScriptValueString->PushString("") : g_pFwdOnGetScriptValueString->PushString(key);
			g_pFwdOnGetScriptValueString->PushStringEx(retValue, sizeof_retValue, SM_PARAM_STRING_COPY, SM_PARAM_COPYBACK);
			g_pFwdOnGetScriptValueString->Execute(&result);
		}

		if(result == Pl_Handled)
		{
			L4D_DEBUG_LOG("CDirector::OnGetScriptValueString will be skipped");
			return NULL;
		}
		else
		{
			return (this->*(GetTrampoline()))(key, defaultValue, retValue, sizeof_retValue);
		}
	}
};
