
#include "player_movement_trace_hull.h"
#include "extension.h"

namespace Detours
{
    void *PlayerMovementTraceHull::OnPlayerMovementTraceHull(void *vector_p1, void *vector_p2, void *vector_p3, void *vector_p4, int uint_p5, void *tracefilter_p6, void *gametrace_p7)
    {
        L4D_DEBUG_LOG("CTerrorGameMovement::PlayerMovementTraceHull() has been called");
        cell_t result = Pl_Continue;

        int hEntity = *(int *)((unsigned char*)gametrace_p7 + 76);    // did the m2 trace hit anyone(i.e. an entity)
		// int entity = -1;
		// if (hEntity != 0)
		// {
			// entity = IndexOfEdict(gameents->BaseEntityToEdict(reinterpret_cast<CBaseEntity *>(hEntity)));
		// }
        
		int client = IndexOfEdict(gameents->BaseEntityToEdict(reinterpret_cast<CBaseEntity *>(*(int *)((unsigned char*)this + 2064))));
		
		g_pSM->LogMessage(myself, "======================");
		g_pSM->LogMessage(myself, "me : %i", client);
		g_pSM->LogMessage(myself, "hEntity : %i", hEntity);
		// L4D_DEBUG_LOG("entity : %i", entity);
		g_pSM->LogMessage(myself, "trace+44 : %f", *(float *)((unsigned char*)gametrace_p7 + 44));
		g_pSM->LogMessage(myself, "trace+55 : %f", *(float *)((unsigned char*)gametrace_p7 + 55));
		
		return (this->*(GetTrampoline()))(vector_p1, vector_p2, vector_p3, vector_p4, uint_p5, tracefilter_p6, gametrace_p7);
		
		// if (g_pFwdOnTerrorWeaponHit && client && entity && userCall)
        // {
            // g_pFwdOnTerrorWeaponHit->PushCell(client); // who shoved
            // g_pFwdOnTerrorWeaponHit->PushCell(entity); // who got shoved
            // g_pFwdOnTerrorWeaponHit->PushCell(weapon); // weapon that's been held while shoving
            // g_pFwdOnTerrorWeaponHit->PushArray(reinterpret_cast<cell_t *>(vector), 3); // shove angles
            // g_pFwdOnTerrorWeaponHit->PushCell(isDeadstop ? true : false); // reliable for high pounces only
            // g_pFwdOnTerrorWeaponHit->Execute(&result);
        // }

        // if(result == Pl_Handled)
        // {
            // L4D_DEBUG_LOG("CTerrorGameMovement::PlayerMovementTraceHull() will be skipped");
            // return NULL;
        // }
        // else
        // {
            // return (this->*(GetTrampoline()))(vector_p1, vector_p2, vector_p3, vector_p4, uint_p5, tracefilter_p6, gametrace_p7);
        // }
    }
};

// cd /home/visor/l4dt/l4dt06final
// sudo make -f Makefile -j8