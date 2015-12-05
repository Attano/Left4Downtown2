
#ifndef _INCLUDE_SOURCEMOD_DETOUR_PLAYER_MOVEMENT_TRACE_HULL_H_
#define _INCLUDE_SOURCEMOD_DETOUR_PLAYER_MOVEMENT_TRACE_HULL_H_

#include "detour_template.h"

namespace Detours {

class PlayerMovementTraceHull;
typedef void * (PlayerMovementTraceHull::*PlayerMovementTraceHullFunc)(void*, void*, void*, void*, int, void*, void*);

class PlayerMovementTraceHull : public DetourTemplate<PlayerMovementTraceHullFunc, PlayerMovementTraceHull>
{
private: //note: implementation of DetourTemplate abstracts

    void *OnPlayerMovementTraceHull(void*, void*, void*, void*, int, void*, void*);

    // get the signature name from the game conf
    virtual const char *GetSignatureName()
    {
        return "CTerrorGameMovement__PlayerMovementTraceHull";
    }

    //notify our patch system which function should be used as the detour
    virtual PlayerMovementTraceHullFunc GetDetour()
    {
        return &PlayerMovementTraceHull::OnPlayerMovementTraceHull;
    }
};

};
#endif
