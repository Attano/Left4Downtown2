#include "replace_tank.h"
#include "extension.h"
#include "util.h"

namespace Detours
{
    void ReplaceTank::OnReplaceTank(CTerrorPlayer* tank, CTerrorPlayer* newtank)
    {
	(this->*(GetTrampoline()))(tank, newtank);
	
	if (g_pFwdOnReplaceTank)
	{
		int old_tank = IndexOfEdict(gameents->BaseEntityToEdict(reinterpret_cast<CBaseEntity *>(tank)));
		int new_tank = IndexOfEdict(gameents->BaseEntityToEdict(reinterpret_cast<CBaseEntity *>(newtank)));

		g_pFwdOnReplaceTank->PushCell(old_tank);
		g_pFwdOnReplaceTank->PushCell(new_tank);
		g_pFwdOnReplaceTank->Execute(NULL);
	}
    }
};
