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

#ifndef _INCLUDE_SOURCEMOD_DETOUR_IS_FINALE_ESCAPE_IN_PROGRESS_H_
#define _INCLUDE_SOURCEMOD_DETOUR_IS_FINALE_ESCAPE_IN_PROGRESS_H_

#include "detour_template.h"

namespace Detours {

// customize these
#define CLASS_NAME         IsFinale
#define L4D_CLASS          CDirector

// do not customize these

#define STR(x) #x
#define TO_STR(x) STR(x)
#define FUNC_TYPE          CLASS_NAME ## Func
#define CALLBACK_NAME      On ## CLASS_NAME
#define FORWARD_NAME       "L4D_On" TO_STR(CLASS_NAME)
#define FUNCTION_FULL_NAME TO_STR(L4D_CLASS) "::" TO_STR(CLASS_NAME)
#define SIGNATURE_NAME     TO_STR(L4D_CLASS) "_" TO_STR(CLASS_NAME)
	
class CLASS_NAME;

typedef bool (CLASS_NAME::*FUNC_TYPE)();

class CLASS_NAME : public DetourTemplate<FUNC_TYPE, CLASS_NAME>
{
protected:

private: //note: implementation of DetourTemplate abstracts

	bool CALLBACK_NAME()
	{
		bool actualInvocationResult = (this->*(GetTrampoline()))();

		return actualInvocationResult;
	}

	// get the signature name (i.e. "IsFinaleEscapeInProgress") from the game conf
	virtual const char *GetSignatureName()
	{
		return SIGNATURE_NAME;
	}

	//notify our patch system which function should be used as the detour
	virtual FUNC_TYPE GetDetour()
	{
		return &CLASS_NAME::CALLBACK_NAME;
	}
};

};

// customize these
#undef CLASS_NAME         
#undef L4D_CLASS          
#undef STR
#undef TO_STR
#undef FUNC_TYPE          
#undef CALLBACK_NAME      
#undef FORWARD_NAME       
#undef FUNCTION_FULL_NAME 
#undef SIGNATURE_NAME     

#endif
