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

enum L4D2BoolMeleeWeaponAttributes
{
	L4D2BMWA_Decapitates
};

enum L4D2IntMeleeWeaponAttributes
{
	L4D2IMWA_DamageFlags,
	L4D2IMWA_RumbleEffect
};

enum L4D2FloatMeleeWeaponAttributes
{
	L4D2FMWA_Damage,
	L4D2FMWA_RefireDelay,
	L4D2FMWA_WeaponIdleTime
};

bool * BoolIdToAttr(CMeleeWeaponInfo *pInfo, int id)
{
	if(pInfo == NULL) return NULL;
	
	switch(id)
	{
		case L4D2BMWA_Decapitates:
			return &pInfo->m_bDecapitates;
		default:
			return NULL;
	}
}

int * IntIdToAttr(CMeleeWeaponInfo *pInfo, int id)
{
	if(pInfo == NULL) return NULL;
	
	switch(id)
	{
		case L4D2IMWA_DamageFlags:
			return &pInfo->m_iDamageFlags;
		case L4D2IMWA_RumbleEffect:
			return &pInfo->m_iRumbleEffect;
		default:
			return NULL;
	}
}

float * FloatIdToAttr(CMeleeWeaponInfo *pInfo, int id)
{
	if(pInfo == NULL) return NULL;
	
	switch(id)
	{
		case L4D2FMWA_Damage:
			return &pInfo->m_fDamage;
		case L4D2FMWA_RefireDelay:
			return &pInfo->m_fRefireDelay;
		case L4D2FMWA_WeaponIdleTime:
			return &pInfo->m_fWeaponIdleTime;
		default:
			return NULL;
	}
}

/*
CMeleeWeaponInfo * StringToId(const char *weapon)
{
	FOR_EACH_VEC((g_pMeleeWeaponInfoStore->weaponNames), i)
	{
		if(strcmp(weapon, g_pMeleeWeaponInfoStore->weaponNames[i]) == 0)
		{
			return g_pMeleeWeaponInfoStore->weaponInfo[i];
		}
	}
	return NULL;
}
*/

CMeleeWeaponInfo * IndexToId(int i)
{
	if (g_pMeleeWeaponInfoStore->weaponInfo.IsValidIndex(i))
	{
		return g_pMeleeWeaponInfoStore->weaponInfo[i];
	}
	else
	{
		return NULL;
	}
}

/*************************************
	Melee Weapon Natives 
	***********************************/

// native L4D2_GetMeleeWeaponIndex(const String:weaponName[]);	
cell_t L4D2_GetMeleeWeaponIndex(IPluginContext *pContext, const cell_t *params)
{
	if (g_pMeleeWeaponInfoStore == NULL)
	{
		return pContext->ThrowNativeError("MeleeWeaponInfoStore unavailable or unsupported. File a bug report.");
	}

	char * weapon = NULL;
	pContext->LocalToString(params[1], &weapon);

	FOR_EACH_VEC( (g_pMeleeWeaponInfoStore->weaponNames), i )
	{
		if(strcmp(weapon, g_pMeleeWeaponInfoStore->weaponNames[i]) == 0)
		{
			return i;
		}
	}
	return -1;
}

// native L4D2_GetIntMeleeAttribute(int id, L4D2IntMeleeWeaponAttribute:attr);
cell_t L4D2_GetIntMeleeAttribute(IPluginContext *pContext, const cell_t *params)
{
	if (g_pMeleeWeaponInfoStore == NULL)
	{
		return pContext->ThrowNativeError("MeleeWeaponInfoStore unavailable or unsupported. File a bug report.");
	}

	CMeleeWeaponInfo * pInfo = IndexToId(params[1]);
	if (pInfo == NULL)
	{
		return pContext->ThrowNativeError("MeleeWeaponInfoStore does not contain index %i.", params[1]);
	}

	int * attr = IntIdToAttr(pInfo, params[2]);
	if(attr == NULL)
	{
		return pContext->ThrowNativeError("Invalid attribute id");
	}
	return *attr;
}

//native Float:L4D2_GetFloatMeleeAttribute(int id, L4D2FloatMeleeWeaponAttributes:attr);
cell_t L4D2_GetFloatMeleeAttribute(IPluginContext *pContext, const cell_t *params)
{
	if (g_pMeleeWeaponInfoStore == NULL)
	{
		return pContext->ThrowNativeError("MeleeWeaponInfo unavailable or unsupported. File a bug report.");
	}

	CMeleeWeaponInfo * pInfo = IndexToId(params[1]);
	if (pInfo == NULL)
	{
		return pContext->ThrowNativeError("MeleeWeaponInfoStore does not contain index %i.", params[1]);
	}

	float * attr = FloatIdToAttr(pInfo, params[2]);
	if(attr == NULL)
	{
		return pContext->ThrowNativeError("Invalid attribute id");
	}
	return sp_ftoc(*attr);
}

// native L4D2_SetIntMeleeAttribute(int id, L4D2IntMeleeWeaponAttributes:attr, value);
cell_t L4D2_SetIntMeleeAttribute(IPluginContext *pContext, const cell_t *params)
{
	if (g_pMeleeWeaponInfoStore == NULL)
	{
		return pContext->ThrowNativeError("MeleeWeaponInfo unavailable or unsupported. File a bug report.");
	}

	CMeleeWeaponInfo * pInfo = IndexToId(params[1]);
	if (pInfo == NULL)
	{
		return pContext->ThrowNativeError("MeleeWeaponInfoStore does not contain index %i.", params[1]);
	}
	
	int * attr = IntIdToAttr(pInfo, params[2]);
	if(attr == NULL)
	{
		return pContext->ThrowNativeError("Invalid attribute id");
	}
	*attr=params[3];
	return 0;
}

// native L4D2_SetFloatMeleeAttribute(int id, L4D2FloatMeleeWeaponAttributes:attr, Float:value);
cell_t L4D2_SetFloatMeleeAttribute(IPluginContext *pContext, const cell_t *params)
{
	if (g_pMeleeWeaponInfoStore == NULL)
	{
		return pContext->ThrowNativeError("MeleeWeaponInfo unavailable or unsupported. File a bug report.");
	}

	CMeleeWeaponInfo * pInfo = IndexToId(params[1]);
	if (pInfo == NULL)
	{
		return pContext->ThrowNativeError("MeleeWeaponInfoStore does not contain index %i.", params[1]);
	}
	
	float * attr = FloatIdToAttr(pInfo, params[2]);
	if(attr == NULL)
	{
		return pContext->ThrowNativeError("Invalid attribute id");
	}
	*attr=sp_ctof(params[3]);
	return 0;
}

// native bool:L4D2_GetBoolMeleeAttribute(int id, L4D2BoolMeleeWeaponAttribute:attr);
cell_t L4D2_GetBoolMeleeAttribute(IPluginContext *pContext, const cell_t *params)
{
	if (g_pMeleeWeaponInfoStore == NULL)
	{
		return pContext->ThrowNativeError("MeleeWeaponInfoStore unavailable or unsupported. File a bug report.");
	}

	CMeleeWeaponInfo * pInfo = IndexToId(params[1]);
	if (pInfo == NULL)
	{
		return pContext->ThrowNativeError("MeleeWeaponInfoStore does not contain index %i.", params[1]);
	}

	int * attr = IntIdToAttr(pInfo, params[2]);
	if(attr == NULL)
	{
		return pContext->ThrowNativeError("Invalid attribute id");
	}
	return (*attr!= 0);
}

// native L4D2_SetBoolMeleeAttribute(int id, L4D2BoolMeleeWeaponAttributes:attr, bool:value);
cell_t L4D2_SetBoolMeleeAttribute(IPluginContext *pContext, const cell_t *params)
{
	if (g_pMeleeWeaponInfoStore == NULL)
	{
		return pContext->ThrowNativeError("MeleeWeaponInfo unavailable or unsupported. File a bug report.");
	}

	CMeleeWeaponInfo * pInfo = IndexToId(params[1]);
	if (pInfo == NULL)
	{
		return pContext->ThrowNativeError("MeleeWeaponInfoStore does not contain index %i.", params[1]);
	}
	
	bool * attr = BoolIdToAttr(pInfo, params[2]);
	if(attr == NULL)
	{
		return pContext->ThrowNativeError("Invalid attribute id");
	}
	*attr = (params[3] != 0);
	return 0;
}

sp_nativeinfo_t g_L4DoMeleeWeaponNatives[] = 
{
	{"L4D2_GetMeleeWeaponIndex", L4D2_GetMeleeWeaponIndex},
	{"L4D2_GetIntMeleeAttribute",	L4D2_GetIntMeleeAttribute},
	{"L4D2_GetFloatMeleeAttribute",	L4D2_GetFloatMeleeAttribute},
	{"L4D2_GetBoolMeleeAttribute",	L4D2_GetBoolMeleeAttribute},
	{"L4D2_SetIntMeleeAttribute",	L4D2_SetIntMeleeAttribute},
	{"L4D2_SetFloatMeleeAttribute",	L4D2_SetFloatMeleeAttribute},
	{"L4D2_SetBoolMeleeAttribute",	L4D2_SetBoolMeleeAttribute},
	{NULL,										NULL}
};
