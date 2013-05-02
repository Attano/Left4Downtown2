//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $NoKeywords: $
//===========================================================================//

#ifndef CONVAR_PUBLIC_H
#define CONVAR_PUBLIC_H
//-----------------------------------------------------------------------------
// Purpose: A console variable, with all its class fields public
//-----------------------------------------------------------------------------
class ConVarPublic : public ConCommandBase, public IConVar
{
friend class CCvar;
friend class ConVarRef;

public:
	typedef ConCommandBase BaseClass;



private:


public:

	// This either points to "this" or it points to the original declaration of a ConVar.
	// This allows ConVars to exist in separate modules, and they all use the first one to be declared.
	// m_pParent->m_pParent must equal m_pParent (ie: m_pParent must be the root, or original, ConVar).
	ConVarPublic						*m_pParent;		//this offset 0x1C
		//personal note: disassembled server code will load always CVARs as cvarPtr->m_pParent

	// Static data
	const char					*m_pszDefaultValue;		//this offset 0x20
	
	// Value
	// Dynamically allocated
	char						*m_pszString;			//this offset 0x24
	int							m_StringLength;			//this offset 0x28

	// Values
	float						m_fValue;				//this offset 0x2C
	int							m_nValue;				//this offset 0x30

	// Min/Max values
	bool						m_bHasMin;				//this offset 0x34
	float						m_fMinVal;				//this offset 0x38
	bool						m_bHasMax;				//this offset 0x3C
	float						m_fMaxVal;				//this offset 0x40
	
	// Call this function when ConVar changes
	FnChangeCallback_t			m_fnChangeCallback;		//this offset 0x44
};

#endif
