#ifndef _INCLUDE_SOURCEMOD_DETOUR_ON_WATER_MOVE_H_
#define _INCLUDE_SOURCEMOD_DETOUR_ON_WATER_MOVE_H_

#include "detour_template.h"

namespace Detours {

class WaterMove;

typedef int (WaterMove::*WaterMoveFunc)();

class WaterMove: public DetourTemplate<WaterMoveFunc, WaterMove>
{
private: //note: implementation of DetourTemplate abstracts

	int OnWaterMove();

	// get the signature name (i.e. "GetCrouchTopSpeed") from the game conf
	virtual const char *GetSignatureName()
	{
		return "WaterMove";
	}

	//notify our patch system which function should be used as the detour
	virtual WaterMoveFunc GetDetour()
	{
		return &WaterMove::OnWaterMove;
	}
};

};
#endif