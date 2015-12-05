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

#include "terror_weapon_hit.h"
#include "extension.h"

namespace Detours
{
    void *TerrorWeaponHit::OnTerrorWeaponHit(CGameTrace *trace/* a1 */, void *vector/* a2 */, bool userCall/* a3 */)
    {
        L4D_DEBUG_LOG("CTerrorWeapon::OnHit() has been called");
        cell_t result = Pl_Continue;

        int hEntity = *(int *)((unsigned char*)trace + 76);    // did the m2 trace hit anyone(i.e. an entity)
        /*  
            deadstop check: see if it's going to be versus_shove_hunter_fov_pouncing(true) or versus_shove_hunter_fov(false)
            often returns 0 when it shouldn't  - either this shit is unreliable, or the game is buggy as fuck
            probably both
        */
        int isDeadstop = *(int *)((unsigned char *)hEntity + 16024);
        int weapon = IndexOfEdict(gameents->BaseEntityToEdict(reinterpret_cast<CBaseEntity *>(this)));
        int entity = IndexOfEdict(gameents->BaseEntityToEdict(reinterpret_cast<CBaseEntity *>(hEntity)));
        CBaseHandle &weaponOwner = *(CBaseHandle *)((unsigned char *)this + 5108);  // get the weapon's owner and thus check its validity(shoves are a secondary attack of anything you're able to hold, even pills and cola)
        int client = !weaponOwner.IsValid() ? -1 : weaponOwner.GetEntryIndex();   // very simplistic and unreliable check, but meh
        /* there's another check being performed here to see if the current gamemode allows bashing... we don't need it */
        if (g_pFwdOnTerrorWeaponHit && client && entity && userCall)
        {
            g_pFwdOnTerrorWeaponHit->PushCell(client); // who shoved
            g_pFwdOnTerrorWeaponHit->PushCell(entity); // who got shoved
            g_pFwdOnTerrorWeaponHit->PushCell(weapon); // weapon that's been held while shoving
            g_pFwdOnTerrorWeaponHit->PushArray(reinterpret_cast<cell_t *>(vector), 3); // shove angles
            g_pFwdOnTerrorWeaponHit->PushCell(isDeadstop ? true : false); // reliable for high pounces only
            g_pFwdOnTerrorWeaponHit->Execute(&result);
        }

        if(result == Pl_Handled)
        {
            L4D_DEBUG_LOG("CTerrorWeapon::OnHit() will be skipped");
            return NULL;
        }
        else
        {
            return (this->*(GetTrampoline()))(trace, vector, userCall);
        }
    }
};
