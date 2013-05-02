/**
 * vim: set ts=4 :
 * =============================================================================
 * Left 4 Downtown SourceMod Extension
 * Copyright (C) 2010 Michael "ProdigySim" Busby
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *
 * Version: $Id$
 */

#ifndef _INCLUDE_CMELEEWEAPONINFO_H_
#define _INCLUDE_CMELEEWEAPONINFO_H_
#include "utlvector.h"
#include "utlmap.h"
// I'm sorry :(
class CBasePlayer;
class CUserCmd;

#include "igamesystem.h"

//typedef char[48] melee_anim_t;

typedef struct CMeleeWeaponInfo_t { // 3232 bytes
	void * pVtable;
	float m_fRefireDelay;
	char m_aViewModel[80];
	char m_aPlayerModel[80];
	char m_aAddonAttachment[80];
	Vector m_vAddonOffest; 
	Vector m_vAddonAngles; 
	char m_aAnimPrefix[16];
	float m_fDamage;
	int m_iDamageFlags;
	int m_iRumbleEffect;
	bool m_bDecapitates;
	char m_aActivityIdle[80];
	char m_aActivityWalk[80];
	char m_aActivityRun[80];
	char m_aActivityCrouchIdle[80];
	char m_aActivityCrouchWalk[80];
	char m_aActivityCrouchWalk2[80]; // possibly run + walk animations
	char m_aActivityIdleInjured[80];
	char m_aActivityWalkInjured[80];
	char m_aActivityRunInjured[80];
	char m_aActivityIdleCalm[80];
	char m_aActivityWalkCalm[80];
	char m_aActivityRunCalm[80];
	char m_aActivityJump[80];
	char m_aActivityDeploy[80];
	char m_aActivityShove[80];
	char SoundData[20][80]; // 1501
	// 3101: 3 bytes should be padding 
	CUtlVector<char[48]> PrimaryAttacks; //3104
	CUtlVector<char[48]> SecondaryAttacks; //3124
	int m_iPlayerAnimEvent; // 3144
	float m_fWeaponIdleTime; // 3148
	char src[80]; // 3152 name of the melee weapon
} CMeleeWeaponInfo;
	
class CMeleeWeaponInfoStore : public CAutoGameSystem // 68 bytes?!?!?
{
	public:
	CUtlMap<int, CMeleeWeaponInfo *, int> weaponInfo; // 12 ?
	CUtlVector<const char *> weaponNames; // 48
};

#endif //_INCLUDE_CMELEEWEAPONINFO_H_
