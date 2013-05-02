//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "extension.h"
#include "l4d2timers.h"

// work-around since client header doesn't like inlined gpGlobals->curtime
float IntervalTimer::Now( void ) const
{
	return gpGlobals->curtime;
}

// work-around since client header doesn't like inlined gpGlobals->curtime
float CountdownTimer::Now( void ) const
{
	return gpGlobals->curtime;
}
