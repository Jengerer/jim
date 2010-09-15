//==========================  Open Steamworks  ================================
//
// This file is part of the Open Steamworks project. All individuals associated
// with this project do not claim ownership of the contents
// 
// The code, comments, and all related files, projects, resources,
// redistributables included with this project are Copyright Valve Corporation.
// Additionally, Valve, the Valve logo, Half-Life, the Half-Life logo, the
// Lambda logo, Steam, the Steam logo, Team Fortress, the Team Fortress logo,
// Opposing Force, Day of Defeat, the Day of Defeat logo, Counter-Strike, the
// Counter-Strike logo, Source, the Source logo, and Counter-Strike Condition
// Zero are trademarks and or registered trademarks of Valve Corporation.
// All other trademarks are property of their respective owners.
//
//=============================================================================

#ifndef ECURRENCYCODE_H
#define ECURRENCYCODE_H
#ifdef _WIN32
#pragma once
#endif

#include "EnumString.h"


typedef enum ECurrencyCode
{
	k_ECurrencyCodeInvalid = 0,
	k_ECurrencyCodeUSD = 1,
	k_ECurrencyCodeGBP = 2,
	k_ECurrencyCodeEUR = 3,
	k_ECurrencyCodeMax = 4,
} ECurrencyCode;

Begin_Enum_String( ECurrencyCode )
{
	Enum_String( k_ECurrencyCodeInvalid );
	Enum_String( k_ECurrencyCodeUSD );
	Enum_String( k_ECurrencyCodeGBP );
	Enum_String( k_ECurrencyCodeEUR );
	Enum_String( k_ECurrencyCodeMax );
}
End_Enum_String( ECurrencyCode );

#endif // ECURRENCYCODE_H
