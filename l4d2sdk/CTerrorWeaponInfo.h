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

#ifndef _INCLUDE_CTERRORWEAPONINFO_H_
#define _INCLUDE_CTERRORWEAPONINFO_H_
#include <weapon_parse.h>
#include "utldict.h"

class CCSWeaponInfo : public FileWeaponInfo_t { // 866 bytes
public:
	float m_fMaxPlayerSpeed; // 2192
	int m_iWeaponType;  // 2196
	int m_iTeam; // 2200 // ANY = 0, TR = 2, CT = 3
	float m_fBotAudiblerange; // 2204
	float m_fWeaponArmorRatio; // 2208
	int m_iCrosshairMinDistance; // 2212
	int m_iCrosshariDeltaDistance; // 2216
	bool m_bCanEquipWithShield; // 2220
	char m_aWrongTeamMsg[32]; // 2221
	char m_aPlayerAnimationExtension[16]; // 2253
	char m_aShieldViewModel[64]; // 2269
	char m_aAddonModel[80]; // 2333
	char m_aAddonAttachment[80]; // 2413
	int m_iMuzzleFlashStyle; // 2496 (not confirmed)
	float m_fMuzzleFlashScale; // 2500
	int m_iPenetration; // 2504
	int m_iDamage; // 2508
	float m_fRange; // 2512
	float m_fRangeModifier; // 2516
	int m_iBullets; // 2520
	float m_fCycleTime; // 2524
	int m_iAccuracyQuadratic; // 2528
	float m_fAccuracyDivisor; // 2532
	float m_fAccuracyOffset; // 2536
	float m_fMaxInaccuracy; // 2540
	float m_fTimeToIdle; // 2544
	float m_fIdleInterval; // 2548
	float m_fNoiseFactor; // 2552
	int m_iBlackMarketPrice; // 2556
	int m_iBlackMarketPrice2; // 2560
	int m_iBlackMarketPreviousPrice; // 2564 (not confirmed)
	char m_aResponseRulesName[80]; // 2568
	char m_aMuzzleFlashEffect_1stPerson[80]; // 2648
	char m_aMuzzleFlashEffect_3rdPerson[80]; // 2728
	char m_aEjectBrassEffect[80]; // 2808
	char m_aDisplayName[80]; // 2888
	char m_aDisplayNameAllCaps[80]; // 2968
	int m_iTier; // 3048
	float m_fMusicDynamicSpeed; // 3052
	bool m_bNewInL4D2; // 3056
	bool m_bCSWeapon; // 3057
};  

class CTerrorWeaponInfo : public CCSWeaponInfo { 
public:
	int m_iLoadoutSlots; // 3060
	float m_fMaxAutoAimDeflection1; // 3064
	float m_fMaxAutoAimRange1; // 3068
	float m_fWeaponAutoAimScale; // 3072
	float m_fVerticalPunch; // 3076
	float m_fHorizontalPunch; // 3080
	float m_fHorizontalPunchDirChance; // 3084
	float m_fSpreadPerShot; // 3088
	float m_fMaxSpread; // 3092
	float m_fSpreadDecay; // 3096
	float m_fMinDuckingSpread; // 3100
	float m_fMinStandingSpread; // 3104
	float m_fMinInAirSpread; // 3108
	float m_fMaxMovementSpread; // 3112
	float m_fPelletScatterPitch; // 3116
	float m_fPelletScatterYaw; // 3120
	float m_fUnknown; // 3124
	float m_fReloadDuration; // 3128
	float m_fDualReloadDuration; // 3132
	float m_fDeployDuration; // 3136
	float m_fDualDeployDuration; // 3140
	float m_fPenetrationNumLayers; // 3144
	float m_fPenetrationPower; // 3148
	float m_fPenetrationMaxDistance; // 3152
	float m_fCharacterPenetrationMaxDistance; // 3156
	float m_fGainRange; // 3160
	CUtlDict<const char*, unsigned short> m_tCharacterViewmodelAddon; // 3164
	char m_aViewModelDual[80]; // 3192
	char m_aPlayerModelDual[80]; // 3272
	char m_aAddonAttachment2[80]; // 3352 Why is this duplicated from 2413
	Vector m_vAddonOffest; // 3432 vector
	Vector m_vAddonAngles; // 3444 vector
	char m_aWorldModel[80]; // 3456
}; 

typedef CUtlDict< CTerrorWeaponInfo *, unsigned short > WeaponDatabase;

#endif //_INCLUDE_CTERRORWEAPONINFO_H_
