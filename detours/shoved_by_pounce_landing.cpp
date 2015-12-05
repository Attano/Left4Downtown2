#include "shoved_by_pounce_landing.h"
#include "extension.h"
#include "util.h"

namespace Detours
{
    void *ShovedByPounceLanding::OnShovedByPounceLanding(CTerrorPlayer* jockeyOrHunter)
    {
		cell_t result = Pl_Continue;
		if (g_pFwdOnShovedByPounceLanding)
		{
			int victim = IndexOfEdict(gameents->BaseEntityToEdict(reinterpret_cast<CBaseEntity *>(this)));
			int attacker = IndexOfEdict(gameents->BaseEntityToEdict(reinterpret_cast<CBaseEntity *>(jockeyOrHunter)));

			g_pFwdOnShovedByPounceLanding->PushCell(victim);
			g_pFwdOnShovedByPounceLanding->PushCell(attacker);
			g_pFwdOnShovedByPounceLanding->Execute(&result);
		}

		if (result == Pl_Handled)
		{
			return NULL;
		}
		else
		{
			return (this->*(GetTrampoline()))(jockeyOrHunter);
		}
    }
};