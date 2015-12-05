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

#include "extension.h"
#include "addons_disabler.h"
#include "CDetour/detourhelpers.h"

static void *vanillaModeSig = NULL;
static patch_t vanillaModeSigRestore;
    
extern ConVar g_AddonsEclipse;

int AddonsDisabler::AddonsEclipse;

void AddonsDisabler::Patch()
{
    L4D_DEBUG_LOG("AddonsDisabler - Patching ...");
    
    bool firstTime = (vanillaModeSig == NULL);
    if (firstTime)
    {
        if (!g_pGameConf->GetMemSig("VanillaModeOffset", &vanillaModeSig) || !vanillaModeSig) 
        { 
            g_pSM->LogError(myself, "AddonsDisabler -- Could not find 'VanillaModeOffset' signature");
            return;
        } 
    }
    patch_t vanillaModePatch;
    vanillaModePatch.bytes = 3;

    // mov esi+19h -> NOP
    vanillaModePatch.patch[0] = 0x0f;
    vanillaModePatch.patch[1] = 0x1f;
    vanillaModePatch.patch[2] = 0x00;

    ApplyPatch(vanillaModeSig, /*offset*/4, &vanillaModePatch, /*restore*/firstTime ? &vanillaModeSigRestore : NULL);
    L4D_DEBUG_LOG("AddonsDisabler -- 'VanillaModeOffset' patched to NOP");
}

void AddonsDisabler::Unpatch()
{
    L4D_DEBUG_LOG("AddonsDisabler - Unpatching ...");

    if (vanillaModeSig)
    {
        ApplyPatch(vanillaModeSig, /*offset*/4, &vanillaModeSigRestore, /*restore*/NULL);
        L4D_DEBUG_LOG("AddonsDisabler -- 'VanillaModeOffset' restored");
    }
}

void OnAddonsEclipseChanged( IConVar *var, const char *pOldValue, float flOldValue )
{
    if (AddonsDisabler::AddonsEclipse == g_AddonsEclipse.GetInt())
        return;

    AddonsDisabler::AddonsEclipse = g_AddonsEclipse.GetInt();
    L4D_DEBUG_LOG("CVAR l4d2_addons_eclipse changed to %i...", AddonsDisabler::AddonsEclipse);
    
    if (AddonsDisabler::AddonsEclipse > -1)
    {
        L4D_DEBUG_LOG("Enabling AddonsDisabler patch");
        AddonsDisabler::Patch();
    }
    else
    {
        L4D_DEBUG_LOG("Disabling AddonsDisabler patch");
        AddonsDisabler::Unpatch();
    }
}

namespace Detours
{
    void CBaseServer::OnFillServerInfo(int SVC_ServerInfo)
    {
        cell_t result = Pl_Continue;
    
        if (g_pFwdAddonsDisabler && AddonsDisabler::AddonsEclipse > 0 && vanillaModeSig)
        {
            int m_nPlayerSlot = *(int *)((unsigned char *)SVC_ServerInfo + 48);
            IClient *pClient = g_pServer->GetClient(m_nPlayerSlot);

            L4D_DEBUG_LOG("ADDONS DISABLER: Eligible client '%s' connected[%s]", pClient->GetClientName(), pClient->GetNetworkIDString());
            
            g_pFwdAddonsDisabler->PushString(pClient->GetNetworkIDString());
            g_pFwdAddonsDisabler->Execute(&result);
            
            /* uint8_t != unsigned char in terms of type */
            uint8_t disableAddons = result == Pl_Handled ? 0 : 1;
            memset((unsigned char *)SVC_ServerInfo + 25, disableAddons, sizeof(uint8_t));
        }

        (this->*(GetTrampoline()))(SVC_ServerInfo);
    }
};