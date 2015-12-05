#ifndef _INCLUDE_SOURCEMOD_DETOUR_INFERNO_SPREAD_H_
#define _INCLUDE_SOURCEMOD_DETOUR_INFERNO_SPREAD_H_

#include "detour_template.h"

namespace Detours {

class InfernoSpread;
typedef void * (InfernoSpread::*InfernoSpreadFunc)(void*, void*, int, int);

class InfernoSpread : public DetourTemplate<InfernoSpreadFunc, InfernoSpread>
{
private: //note: implementation of DetourTemplate abstracts

    void *OnInfernoSpread(void*, void*, int, int);

    // get the signature name from the game conf
    virtual const char *GetSignatureName()
    {
		return "CInferno__Spread";
    }

    //notify our patch system which function should be used as the detour
    virtual InfernoSpreadFunc GetDetour()
    {
        return &InfernoSpread::OnInfernoSpread;
    }
};

};
#endif
