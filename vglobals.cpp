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

#include "vglobals.h"
#include "extension.h"
#include "util.h"

void **g_pGameRules = NULL;
void **g_pEngine = NULL;
CDirector **g_pDirector = NULL;
void *g_pZombieManager = NULL;
WeaponDatabase *g_pWeaponInfoDatabase = NULL;
CMeleeWeaponInfoStore *g_pMeleeWeaponInfoStore = NULL;

void InitializeValveGlobals()
{
	char *addr = NULL;
#ifdef PLATFORM_WINDOWS
	int offset;

	/* g_pGameRules */
	if (!g_pGameConfSDKTools->GetMemSig("CreateGameRulesObject", (void **)&addr) || !addr)
	{
		return;
	}
	if (!g_pGameConfSDKTools->GetOffset("g_pGameRules", &offset) || !offset)
	{
		return;
	}
	g_pGameRules = *reinterpret_cast<void ***>(addr + offset);

	/* g_pDirector */
	const char *directorConfKey = "DirectorMusicBanks_OnRoundStart";
	if (!g_pGameConf->GetMemSig(directorConfKey, (void **)&addr) || !addr)
	{
		return;
	}
	if (!g_pGameConf->GetOffset("TheDirector", &offset) || !offset)
	{
		return;
	}
	g_pDirector = *reinterpret_cast<CDirector ***>(addr + offset);
	
	const char *weaponDatabaseConfKey = "ReadWeaponDataFromFileForSlot";
	if (!g_pGameConf->GetMemSig(weaponDatabaseConfKey, (void **)&addr) || !addr)
	{
		L4D_DEBUG_LOG("Unable to find ReadWeaponDataFromFileForSlot signature");
		return;
	}
	if (!g_pGameConf->GetOffset("WeaponInfoDatabase", &offset) || !offset)
	{
		return;
	}
	g_pWeaponInfoDatabase = *reinterpret_cast<WeaponDatabase **>(addr + offset);

	if (!g_pGameConf->GetMemSig("MeleeWeaponInfoStore_Construction", (void **)&addr) || !addr)
	{
		L4D_DEBUG_LOG("Unable to find MeleeWeaponInfoStore signature");
		return;
	}
	if (!g_pGameConf->GetOffset("CMeleeWeaponInfoStore", &offset) || !offset)
	{
		return;
	}
	g_pMeleeWeaponInfoStore = *reinterpret_cast<CMeleeWeaponInfoStore **>(addr + offset);
	
	L4D_DEBUG_LOG("MeleeWeaponInfo Store: %p ", g_pMeleeWeaponInfoStore);
	L4D_DEBUG_LOG("MeleeWeaponInfo Store: %s ", g_pMeleeWeaponInfoStore->Name());
#elif defined PLATFORM_LINUX

	/* g_pGameRules */
	if (!g_pGameConfSDKTools->GetMemSig("g_pGameRules", (void **)&addr) || !addr)
	{
	    L4D_DEBUG_LOG("Couldn't find GameRules instance!");
		return;
	}
	g_pGameRules = reinterpret_cast<void **>(addr);

	/* g_pDirector */
	if (!g_pGameConf->GetMemSig("TheDirector", (void **)&addr) || !addr)
	{
	    L4D_DEBUG_LOG("Couldn't find CDirector instance!");
		return;
	}
	g_pDirector = reinterpret_cast<CDirector **>(addr);

	/* g_pZombieManager */
	if (!g_pGameConf->GetMemSig("TheZombieManager", (void **)&addr) || !addr)
	{
	    L4D_DEBUG_LOG("Couldn't find ZombieManager instance!");
		return;
	}
	g_pZombieManager = reinterpret_cast<void **>(addr);

	/* g_pWeaponInfoDatabase */
	if (!g_pGameConf->GetMemSig("WeaponInfoDatabase", (void **)&addr) || !addr)
	{
	    L4D_DEBUG_LOG("WeaponInfoDatabase signature not found (%p)", addr);
		return;
	}
	g_pWeaponInfoDatabase = reinterpret_cast<WeaponDatabase *>(addr);

	if (!g_pGameConf->GetMemSig("CMeleeWeaponInfoStore", (void **)&addr) || !addr)
	{
	    L4D_DEBUG_LOG("CMeleeWeaponInfoStore signature not found (%p)", addr);
		return;
	}
	g_pMeleeWeaponInfoStore = reinterpret_cast<CMeleeWeaponInfoStore *>(addr);
#endif
	
	if(!g_pGameConf->GetAddress("ZombieManager", (void **)&addr))
	{
		L4D_DEBUG_LOG("TheZombieManager address not found.");
		return;
	}
	g_pZombieManager = addr;
	L4D_DEBUG_LOG("TheZombieManager found at: %p", g_pZombieManager);
}
