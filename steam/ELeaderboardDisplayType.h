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

#ifndef ELEADERBOARDDISPLAYTYPE_H
#define ELEADERBOARDDISPLAYTYPE_H
#ifdef _WIN32
#pragma once
#endif

#include "EnumString.h"

// the display type (used by the Steam Community web site) for a leaderboard
typedef enum ELeaderboardDisplayType
{
	k_ELeaderboardDisplayTypeNone = 0, 
	k_ELeaderboardDisplayTypeNumeric = 1,			// simple numerical score
	k_ELeaderboardDisplayTypeTimeSeconds = 2,		// the score represents a time, in seconds
	k_ELeaderboardDisplayTypeTimeMilliSeconds = 3,	// the score represents a time, in milliseconds
} ELeaderboardDisplayType;

Begin_Enum_String(ELeaderboardDisplayType)
{
	Enum_String(k_ELeaderboardDisplayTypeNone);
	Enum_String(k_ELeaderboardDisplayTypeNumeric);
	Enum_String(k_ELeaderboardDisplayTypeTimeSeconds);
	Enum_String(k_ELeaderboardDisplayTypeTimeMilliSeconds);

}
End_Enum_String(ELeaderboardDisplayType);

#endif // ELEADERBOARDDISPLAYTYPE_H
