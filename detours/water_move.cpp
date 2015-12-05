#include "water_move.h"
#include "extension.h"

namespace Detours
{
	int WaterMove::OnWaterMove()
	{
		cell_t result = Pl_Continue;

		int inWaterMove = (this->*(GetTrampoline()))();
		
		if (g_pFwdOnWaterMove && inWaterMove)
		{
			edict_t *pEntity = gameents->BaseEntityToEdict(reinterpret_cast<CBaseEntity*>(this));
			int client = IndexOfEdict(pEntity);
		
			g_pFwdOnWaterMove->PushCell(client);
			g_pFwdOnWaterMove->Execute(&result);
		}

		return inWaterMove;
	}
};