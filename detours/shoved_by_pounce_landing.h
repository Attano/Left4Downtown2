#ifndef _INCLUDE_SOURCEMOD_DETOUR_SHOVED_BY_POUNCE_LANDING_H_
#define _INCLUDE_SOURCEMOD_DETOUR_SHOVED_BY_POUNCE_LANDING_H_

#include "detour_template.h"

class CTerrorPlayer;

namespace Detours {

class ShovedByPounceLanding;
typedef void * (ShovedByPounceLanding::*ShovedByPounceLandingFunc)(CTerrorPlayer*);

class ShovedByPounceLanding : public DetourTemplate<ShovedByPounceLandingFunc, ShovedByPounceLanding>
{
private: //note: implementation of DetourTemplate abstracts

    void *OnShovedByPounceLanding(CTerrorPlayer*);

    // get the signature name from the game conf
    virtual const char *GetSignatureName()
    {
		return "CTerrorPlayer__OnShovedByPounceLanding";
    }

    //notify our patch system which function should be used as the detour
    virtual ShovedByPounceLandingFunc GetDetour()
    {
        return &ShovedByPounceLanding::OnShovedByPounceLanding;
    }
};

};
#endif