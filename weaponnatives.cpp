/**
 * vim: set ts=4 :
 * =============================================================================
 * Left 4 Downtown 2 SourceMod Extension
 * Copyright (C) 2010 Michael "ProdigySim" Busby.
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
 
#include "extension.h"
#include "vglobals.h"

enum L4D2IntWeaponAttributes
{
	L4D2IWA_Damage,
	L4D2IWA_Bullets,
	L4D2IWA_ClipSize
};

enum L4D2FloatWeaponAttributes
{
	L4D2FWA_MaxPlayerSpeed,
	L4D2FWA_SpreadPerShot,
	L4D2FWA_MaxSpread,
	L4D2FWA_SpreadDecay,
	L4D2FWA_MinDuckingSpread,
	L4D2FWA_MinStandingSpread,
	L4D2FWA_MinInAirSpread,
	L4D2FWA_MaxMovementSpread,
	L4D2FWA_PenetrationNumLayers,
	L4D2FWA_PenetrationPower,
	L4D2FWA_PenetrationMaxDist,
	L4D2FWA_CharPenetrationMaxDist,
	L4D2FWA_Range,
	L4D2FWA_RangeModifier,
	L4D2FWA_CycleTime,
	L4D2FWA_PelletScatterPitch,
	L4D2FWA_PelletScatterYaw
};

int * IntIdToAttr(CTerrorWeaponInfo *pInfo, int id)
{
	if(pInfo == NULL) return NULL;
	
	switch(id)
	{
		case L4D2IWA_Damage:
			return &pInfo->m_iDamage;
		case L4D2IWA_Bullets:
			return &pInfo->m_iBullets;
		case L4D2IWA_ClipSize:
			return &pInfo->iMaxClip1;
		default:
			return NULL;
	}
}
float * FloatIdToAttr(CTerrorWeaponInfo *pInfo, int id)
{
	if(pInfo == NULL) return NULL;
	
	switch(id)
	{
		case L4D2FWA_MaxPlayerSpeed:
			return &pInfo->m_fMaxPlayerSpeed;
		case L4D2FWA_SpreadPerShot:
			return &pInfo->m_fSpreadPerShot;
		case L4D2FWA_MaxSpread:
			return &pInfo->m_fMaxSpread;
		case L4D2FWA_SpreadDecay:
			return &pInfo->m_fSpreadDecay;
		case L4D2FWA_MinDuckingSpread:
			return &pInfo->m_fMinDuckingSpread;
		case L4D2FWA_MinStandingSpread:
			return &pInfo->m_fMinStandingSpread;
		case L4D2FWA_MinInAirSpread:
			return &pInfo->m_fMinInAirSpread;
		case L4D2FWA_MaxMovementSpread:
			return &pInfo->m_fMaxMovementSpread;
		case L4D2FWA_PenetrationNumLayers:
			return &pInfo->m_fPenetrationNumLayers;
		case L4D2FWA_PenetrationPower:
			return &pInfo->m_fPenetrationPower;
		case L4D2FWA_PenetrationMaxDist:
			return &pInfo->m_fPenetrationMaxDistance;
		case L4D2FWA_CharPenetrationMaxDist:
			return &pInfo->m_fCharacterPenetrationMaxDistance;
		case L4D2FWA_Range:
			return &pInfo->m_fRange;
		case L4D2FWA_RangeModifier:
			return &pInfo->m_fRangeModifier;
		case L4D2FWA_CycleTime:
			return &pInfo->m_fCycleTime;
		case L4D2FWA_PelletScatterPitch:
			return &pInfo->m_fPelletScatterPitch;
		case L4D2FWA_PelletScatterYaw:
			return &pInfo->m_fPelletScatterYaw;
		default:
			return NULL;
	}
}

/*************************************
	WeaponInfo Natives 
	***********************************/
// native bool:L4D2_IsValidWeapon(const String:weaponName[]);
cell_t L4D2_IsValidWeapon(IPluginContext *pContext, const cell_t *params)
{
	if (g_pWeaponInfoDatabase == NULL)
	{
		return pContext->ThrowNativeError("WeaponInfoDatabase unavailable or unsupported. File a bug report.");
	}
	
	char * weapon = NULL;
	pContext->LocalToString(params[1], &weapon);

	return g_pWeaponInfoDatabase->IsValidIndex(g_pWeaponInfoDatabase->Find(weapon));
}


// native L4D2_GetIntWeaponAttribute(const String:weaponName[], L4D2IntWeaponAttribute:attr);
cell_t L4D2_GetIntWeaponAttribute(IPluginContext *pContext, const cell_t *params)
{
	if (g_pWeaponInfoDatabase == NULL)
	{
		return pContext->ThrowNativeError("WeaponInfoDatabase unavailable or unsupported. File a bug report.");
	}
	char * weapon = NULL;
	pContext->LocalToString(params[1], &weapon);
	int iIndex = g_pWeaponInfoDatabase->Find(weapon);

	if (!g_pWeaponInfoDatabase->IsValidIndex(iIndex))
	{
		return pContext->ThrowNativeError("Invalid weapon name or weapon unavailable");
	}
	CTerrorWeaponInfo *pInfo = g_pWeaponInfoDatabase->Element(iIndex);
	
	int * attr = IntIdToAttr(pInfo, params[2]);
	if(attr == NULL)
	{
		return pContext->ThrowNativeError("Invalid attribute id");
	}
	return *attr;
}

// native Float:L4D2_GetFloatWeaponAttribute(const String:weaponName[], L4D2IntWeaponAttribute:attr);
cell_t L4D2_GetFloatWeaponAttribute(IPluginContext *pContext, const cell_t *params)
{
	if (g_pWeaponInfoDatabase == NULL)
	{
		return pContext->ThrowNativeError("WeaponInfoDatabase unavailable or unsupported. File a bug report.");
	}
	char * weapon = NULL;
	pContext->LocalToString(params[1], &weapon);
	int iIndex = g_pWeaponInfoDatabase->Find(weapon);

	if (!g_pWeaponInfoDatabase->IsValidIndex(iIndex))
	{
		return pContext->ThrowNativeError("Invalid weapon name or weapon unavailable");
	}
	CTerrorWeaponInfo *pInfo = g_pWeaponInfoDatabase->Element(iIndex);
	
	float * attr = FloatIdToAttr(pInfo, params[2]);
	if(attr == NULL)
	{
		return pContext->ThrowNativeError("Invalid attribute id");
	}
	return sp_ftoc(*attr);
}

// native L4D2_SetIntWeaponAttribute(const String:weaponName[], L4D2IntWeaponAttribute:attr, value);
cell_t L4D2_SetIntWeaponAttribute(IPluginContext *pContext, const cell_t *params)
{
	if (g_pWeaponInfoDatabase == NULL)
	{
		return pContext->ThrowNativeError("WeaponInfoDatabase unavailable or unsupported. File a bug report.");
	}
	char * weapon = NULL;
	pContext->LocalToString(params[1], &weapon);
	int iIndex = g_pWeaponInfoDatabase->Find(weapon);

	if (!g_pWeaponInfoDatabase->IsValidIndex(iIndex))
	{
	    return pContext->ThrowNativeError("Invalid weapon name or weapon unavailable");
	}
	CTerrorWeaponInfo *pInfo = g_pWeaponInfoDatabase->Element(iIndex);
	
	int * attr = IntIdToAttr(pInfo, params[2]);
	if(attr == NULL)
	{
		return pContext->ThrowNativeError("Invalid attribute id");
	}
	*attr=params[3];
	return 0;
}

// native Float:L4D2_SetFloatWeaponAttribute(const String:weaponName[], L4D2IntWeaponAttribute:attr, Float:value);
cell_t L4D2_SetFloatWeaponAttribute(IPluginContext *pContext, const cell_t *params)
{
	if (g_pWeaponInfoDatabase == NULL)
	{
		return pContext->ThrowNativeError("WeaponInfoDatabase unavailable or unsupported. File a bug report.");
	}
	char * weapon = NULL;
	pContext->LocalToString(params[1], &weapon);
	int iIndex = g_pWeaponInfoDatabase->Find(weapon);

	if (!g_pWeaponInfoDatabase->IsValidIndex(iIndex))
	{
	    return pContext->ThrowNativeError("Invalid weapon name or weapon unavailable");
	}
	CTerrorWeaponInfo *pInfo = g_pWeaponInfoDatabase->Element(iIndex);
	
	float * attr = FloatIdToAttr(pInfo, params[2]);
	if(attr == NULL)
	{
		return pContext->ThrowNativeError("Invalid attribute id");
	}
	*attr=sp_ctof(params[3]);
	
	return 0;
}

sp_nativeinfo_t  g_L4DoWeaponNatives[] = 
{
	{"L4D2_IsValidWeapon",			L4D2_IsValidWeapon},
	{"L4D2_GetIntWeaponAttribute",	L4D2_GetIntWeaponAttribute},
	{"L4D2_GetFloatWeaponAttribute",	L4D2_GetFloatWeaponAttribute},
	{"L4D2_SetIntWeaponAttribute",	L4D2_SetIntWeaponAttribute},
	{"L4D2_SetFloatWeaponAttribute",	L4D2_SetFloatWeaponAttribute},
	{NULL,										NULL}
};
