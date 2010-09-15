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

#ifndef ISTEAMGAMECOORDINATORCOMMON_H
#define ISTEAMGAMECOORDINATORCOMMON_H
#ifdef _WIN32
#pragma once
#endif

#include "EnumString.h"

#define STEAMGAMECOORDINATOR_INTERFACE_VERSION_001 "SteamGameCoordinator001"

struct GameCoordinatorMessageAvailable_t
{
	enum { k_iCallback = k_iSteamGameCoordinatorCallbacks + 1 };
	uint32 messageLength;
};

enum EGCMessages
{
	k_ESOMsg_Create = 21,
	k_ESOMsg_Update,
	k_ESOMsg_Destroy,
	k_ESOMsg_CacheSubscribed,
	k_ESOMsg_CacheUnsubscribed,

	k_EGCMsgStartPlaying = 53,
	k_EGCMsgStopPlaying,
	k_EGCMsgStartGameserver,
	k_EGCMsgStopGameserver,

	k_EMsgGCSetItemPosition = 1001,
	k_EMsgGCCraft,
	k_EMsgGCCraftResponse,
	k_EMsgGCDelete,
	k_EMsgGCVerifyCacheSubscription,

	k_EMsgGCDev_NewItemRequest = 2001,
	k_EMsgGCDev_NewItemRequestResponse
};

Begin_Enum_String(EGCMessages)
{
	Enum_String( k_ESOMsg_Create );
	Enum_String( k_ESOMsg_Update );
	Enum_String( k_ESOMsg_Destroy );
	Enum_String( k_ESOMsg_CacheSubscribed );
	Enum_String( k_ESOMsg_CacheUnsubscribed );

	Enum_String( k_EGCMsgStartPlaying );
	Enum_String( k_EGCMsgStopPlaying );
	Enum_String( k_EGCMsgStartGameserver );
	Enum_String( k_EGCMsgStopGameserver );

	Enum_String( k_EMsgGCSetItemPosition );
	Enum_String( k_EMsgGCCraft );
	Enum_String( k_EMsgGCCraftResponse );
	Enum_String( k_EMsgGCDelete );

	Enum_String( k_EMsgGCDev_NewItemRequest );
	Enum_String( k_EMsgGCDev_NewItemRequestResponse );
}
End_Enum_String( EGCMessages );


#pragma pack(push, 1)

/*
0100 ffffffffffffffffffffffffffffffff 86cf4e0001001001 01000000 d069ea0200000000 86cf4e00 8f13 01 03 00000000 01000000 0000
0100 ffffffffffffffffffffffffffffffff 86cf4e0001001001 01000000 cff9ea0200000000 86cf4e00 2a00 01 03 00000000 01000000 0000
0100 ffffffffffffffffffffffffffffffff 86cf4e0001001001 01000000 8dbaf70200000000 86cf4e00 8b13 01 03 00000000 01000000 0000
*/
struct SOMsgCreate_t
{
	enum { k_iMessage = k_ESOMsg_Create };
	uint16 id;
	char garbage[16];
	CSteamID steamid;
	uint32 unknown;
	
	//Same as SOMsgCacheSubscribed_Item_t
	uint64 itemid;
	uint32 accountid;
	uint16 itemtype;
	uint8 itemlevel;
	uint8 itemquality;
	uint32 position;
	uint32 itemcount;
	uint16 attribcount;
};


/*
0100 ffffffffffffffffffffffffffffffff 86cf4e0001001001 01000000 76f0da0200000000 0105 0f000080
0100 ffffffffffffffffffffffffffffffff 86cf4e0001001001 01000000 21ccd90200000000 0105 10000080
0100 ffffffffffffffffffffffffffffffff 86cf4e0001001001 01000000 d069ea0200000000 0105 20000080
*/
struct SOMsgUpdate_t
{
	enum { k_iMessage = k_ESOMsg_Update };

	uint16 id;
	char garbage[16];
	CSteamID steamid;
	uint32 unk1;
	uint64 itemID;
	uint16 unk2;
	uint32 position;
};


/*
0100 ffffffffffffffffffffffffffffffff 86cf4e0001001001 01000000 7f7e1b0200000000
0100 ffffffffffffffffffffffffffffffff 86cf4e0001001001 01000000 5a77020200000000
0100 ffffffffffffffffffffffffffffffff 86cf4e0001001001 01000000 bdbc1c0200000000
0100 ffffffffffffffffffffffffffffffff 86cf4e0001001001 01000000 8885210200000000
0100 ffffffffffffffffffffffffffffffff 86cf4e0001001001 01000000 e582e30100000000
*/
struct SOMsgDeleted_t
{
	enum { k_iMessage = k_ESOMsg_Destroy };
	uint16 id;
	char garbage[16];
	CSteamID steamid;
	uint32 unk1;
	uint64 itemid;
};

/*
Too long, log that yourself
*/
struct SOMsgCacheSubscribed_t
{
	enum { k_iMessage = k_ESOMsg_CacheSubscribed };

	uint16 id;
	char garbage[16];
	CSteamID steamid;
	uint32 unknown;
	uint16 padding;
	uint16 itemcount;
};

struct SOMsgCacheSubscribed_Item_t
{
	uint64 itemid;
	uint32 accountid;
	uint16 itemdefindex;
	uint8 itemlevel;
	uint8 itemquality;
	uint32 position;
	uint32 itemcount;
	uint16 attribcount;
};

struct SOMsgCacheSubscribed_Item_Attrib_t
{
	uint16 attribindex;
	float value;
};


struct SOMsgCacheUnsubscribed_t
{
	enum { k_iMessage = k_ESOMsg_CacheUnsubscribed };

	uint16 id;
	char garbage[16];
	CSteamID steamid;
};


/*
0100 ffffffffffffffffffffffffffffffff 76f0da0200000000 0f000080 00000000
0100 ffffffffffffffffffffffffffffffff 21ccd90200000000 10000080 00000000
0100 ffffffffffffffffffffffffffffffff cff9ea0200000000 42000080 00000000
0100 ffffffffffffffffffffffffffffffff d069ea0200000000 20000080 00000000
*/
struct GCSetItemPosition_t
{
	enum { k_iMessage = k_EMsgGCSetItemPosition };
	uint16 id;
	char garbage[16];
	uint64 itemID;
	uint32 position;
	uint32 unk1;
};


/*
This one is 4 natasha
0100 ffffffffffffffffffffffffffffffff 0700 0400 5a77020200000000 bdbc1c0200000000 8885210200000000 e582e30100000000
*/
struct GCCraft_t
{
	enum { k_iMessage = k_EMsgGCCraft };
	uint16 id;
	char garbage[16];
	uint16 blueprint;//0xffff = unknown blueprint
	uint16 itemcount;
	//+ (uint64 itemid)*itemcount
};


/*
0100 ffffffffffffffffffffffffffffffff 0700 0000000000000100 d069ea0200000000
*/
struct GCCraftResponse_t
{
	enum { k_iMessage = k_EMsgGCCraftResponse };
	uint16 id;
	char garbage[16];
	uint16 blueprint;//0xffff = failed
	uint64 unk1;
	uint64 itemid;
};


/*
0100 ffffffffffffffffffffffffffffffff 7f7e1b0200000000
*/
struct GCDelete_t
{
	enum { k_iMessage = k_EMsgGCDelete };
	uint16 id;
	char garbage[16];
	uint64 itemID;
};

/*
0100 ffffffffffffffffffffffffffffffff 86cf4e0001001001
*/
struct GCVerifyCacheSubscription_t
{
	enum { k_iMessage = k_EMsgGCVerifyCacheSubscription };

	uint16 id;
	char garbage[16];
	CSteamID steamid;
};

#pragma pack(pop)

#endif // ISTEAMGAMECOORDINATORCOMMON_H
