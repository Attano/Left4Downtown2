#ifndef _INCLUDE_SOURCEMOD_DETOUR_SHOVED_BY_POUNCE_LANDING_H_
#define _INCLUDE_SOURCEMOD_DETOUR_SHOVED_BY_POUNCE_LANDING_H_

#include "detour_template.h"

class CTerrorPlayer;

namespace Detours {

class ReplaceTank;
typedef void (ReplaceTank::*ReplaceTankFunc)(CTerrorPlayer*, CTerrorPlayer*);

class ReplaceTank : public DetourTemplate<ReplaceTankFunc, ReplaceTank>
{
private: //note: implementation of DetourTemplate abstracts

    void OnReplaceTank(CTerrorPlayer*, CTerrorPlayer*);

    // get the signature name from the game conf
    virtual const char *GetSignatureName()
    {
	return "ReplaceTank";
    }

    //notify our patch system which function should be used as the detour
    virtual ReplaceTankFunc GetDetour()
    {
        return &ReplaceTank::OnReplaceTank;
    }
};

};
#endif
