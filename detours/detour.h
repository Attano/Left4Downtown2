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

#ifndef _INCLUDE_SOURCEMOD_DETOUR_H_
#define _INCLUDE_SOURCEMOD_DETOUR_H_

#include "../extension.h"
#include "../codepatch/icodepatch.h"

struct patch_t;

class Detour : public ICodePatch
{
public:

	//Initialize the Detour classes before ever calling Patch()
	static void Init(ISourcePawnEngine *spengine, IGameConfig *gameconf);

	Detour();
	virtual ~Detour();

	//enable the detour
	void Patch();

	//disable the detour
	void Unpatch();

	// get the signature name (i.e. "SpawnTank") from the game conf
	virtual const char *GetSignatureName() = 0;
	
	virtual unsigned char *GetSignatureAddress() { return NULL; }

protected: //note: implemented by direct superclass

	//call back when detour is successfully enabled
	virtual void OnPatched() = 0;

	//save the trampoline pointer
	virtual void SetTrampoline(void *trampoline) = 0;

	//return a void* to the detour function
	virtual void *GetDetourRaw() = 0;

	//return a void* to the trampoline (after we Set it)
	virtual void *GetTrampolineRaw() = 0;

	static IGameConfig *gameconf;
private:
	bool isPatched;

	unsigned char *signature;

	const char *signatureName;
	patch_t *restore;
	unsigned char *trampoline;


	static ISourcePawnEngine *spengine;

	void PatchFromSignature(const char *signatureName, void *targetFunction, unsigned char *&originalFunction, unsigned char *&signature);
	
	void PatchFromAddress(void *targetFunction, unsigned char *&originalFunction, unsigned char *&signature);

	//insert a specific JMP instruction at the given location, save it to the buffer
	void InjectJmp(void *buffer, void *source, void *destination);
};

#endif
