#include "inferno_spread.h"
#include "extension.h"
#include "util.h"

namespace Detours
{
    void *InfernoSpread::OnInfernoSpread(void *vector1, void *vector2, int i1, int i2)
    {
        CBaseHandle &infernoOwner = *(CBaseHandle *)((unsigned char *)this + 544);
        int thrower = !infernoOwner.IsValid() ? -1 : infernoOwner.GetEntryIndex(); // spitter or other kind of fire-raiser (survivor that has thrown a molotov etc)
		CBaseEntity *target = UTIL_GetCBaseEntity(thrower, true);
		if (*(int *)((unsigned char *)target + 588) != 3 || *(int *)((unsigned char *)target + 11204) != 4) // not infected / not spitter
        {
            return (this->*(GetTrampoline()))(vector1, vector2, i1, i2);
        }
		
		edict_t *pProjectile = gameents->BaseEntityToEdict(reinterpret_cast<CBaseEntity *>(this));
		int projectile = IndexOfEdict(pProjectile);
		// int puddleID = *(int *)((unsigned char *)this + 2624);
		
		float x1 = *(float *)((unsigned char *)vector1);
		float y1 = *(float *)((unsigned char *)vector1 + 4);
		float z1 = *(float *)((unsigned char *)vector1 + 8);
		// float x2 = *(float *)((unsigned char *)vector2);
		// float y2 = *(float *)((unsigned char *)vector2 + 4);
		// float z2 = *(float *)((unsigned char *)vector2 + 8);
		
		cell_t result = Pl_Continue;
		if (g_pFwdInfernoSpread)
        {
			g_pFwdInfernoSpread->PushCell(thrower);
            g_pFwdInfernoSpread->PushCell(projectile);
            // g_pFwdInfernoSpread->PushCell(puddleID);
            g_pFwdInfernoSpread->PushFloatByRef(&x1);
            g_pFwdInfernoSpread->PushFloatByRef(&y1);
            g_pFwdInfernoSpread->PushFloatByRef(&z1);
            // g_pFwdInfernoSpread->PushFloatByRef(&x2);
            // g_pFwdInfernoSpread->PushFloatByRef(&y2);
            // g_pFwdInfernoSpread->PushFloatByRef(&z2);
            g_pFwdInfernoSpread->Execute(&result);
        }

		if (result == Pl_Handled)
        {
            return NULL;
        }
        else
        {
			*(float *)((unsigned char *)vector1) = x1;
			*(float *)((unsigned char *)vector1 + 4) = y1;
			*(float *)((unsigned char *)vector1 + 8) = z1;
			// *(float *)((unsigned char *)vector2) = x2;
			// *(float *)((unsigned char *)vector2 + 4) = y2;
			// *(float *)((unsigned char *)vector2 + 8) = z2;
            return (this->*(GetTrampoline()))(vector1, vector2, i1, i2);
        }
    }
};
