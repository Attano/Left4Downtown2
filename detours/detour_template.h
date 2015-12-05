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

#ifndef _INCLUDE_SOURCEMOD_DETOUR_TEMPLATE_H_
#define _INCLUDE_SOURCEMOD_DETOUR_TEMPLATE_H_

#include "detour.h"
#include "auto_forward.h"
#define MFP_CODE_ADDRESS(mfp) (*(void **)(&(mfp)))

template <typename TDetour, typename TParent>
class DetourTemplate : public Detour
{
protected: //note: virtuals should be implemented by the TParent class

	// get the signature name (i.e. "SpawnTank") from the game conf
	virtual const char *GetSignatureName() = 0;

	//notify our patch system which function should be used as the detour
	virtual TDetour GetDetour() = 0;

	//get the trampoline, use it to call the original function in a detour
	static TDetour GetTrampoline()
	{
		return GetTrampolineRef();
	}

	//onpatched callback
	virtual void OnPatched() { }

	//yes this assumes that we instantiate a Singleton
	DetourTemplate() { GetAutoForwardPtrRef() = NULL; }
	~DetourTemplate() { if(GetAutoForward()) { delete GetAutoForward(); SetAutoForward(NULL); } }

	static void SetAutoForward(AutoForward *autoForward)
	{
		GetAutoForwardPtrRef() = autoForward;
	}

	static AutoForward* GetAutoForward()
	{
		AutoForward *autoFwd = GetAutoForwardPtrRef();
		return autoFwd;
	}

private: 

	static AutoForward*& GetAutoForwardPtrRef()
	{
		static AutoForward* autoForward = NULL;
		return autoForward;
	}

	//get the trampoline ref so we can change the value later
	static TDetour& GetTrampolineRef()
	{
		static TDetour trampoline = NULL;
		return trampoline;
	}

//note: implementation of Detour abstract virtual functions

	//save the trampoline pointer
	virtual void SetTrampoline(void *trampoline)
	{
		MFP_CODE_ADDRESS(GetTrampolineRef()) = trampoline;
	}

	//return a void* to the detour function
	virtual void *GetDetourRaw()
	{
		TDetour detour = GetDetour();
		return MFP_CODE_ADDRESS(detour);
	}

	//return a void* to the trampoline (after we Set it)
	virtual void *GetTrampolineRaw()
	{
		TDetour trampoline = GetTrampoline();
		return MFP_CODE_ADDRESS(trampoline);
	}
};

#undef MFP_CODE_ADDRESS

#endif
