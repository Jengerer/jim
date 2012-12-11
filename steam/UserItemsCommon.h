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

#ifndef USERITEMSCOMMON_H
#define USERITEMSCOMMON_H
#ifdef _WIN32
#pragma once
#endif



#define STEAMUSERITEMS_INTERFACE_VERSION_001 "STEAMUSERITEMS_INTERFACE_VERSION001"
#define STEAMUSERITEMS_INTERFACE_VERSION_002 "STEAMUSERITEMS_INTERFACE_VERSION002"
#define STEAMUSERITEMS_INTERFACE_VERSION_003 "STEAMUSERITEMS_INTERFACE_VERSION003"



typedef enum EItemCriteriaOperator
{
	k_EOperator_String_EQ,
	k_EOperator_Not, 
	k_EOperator_String_Not_EQ = 1,
	k_EOperator_Float_EQ,
	k_EOperator_Float_Not_EQ, 
	k_EOperator_Float_LT,
	k_EOperator_Float_Not_LT,
	k_EOperator_Float_LTE, 
	k_EOperator_Float_Not_LTE,
	k_EOperator_Float_GT,
	k_EOperator_Float_Not_GT, 
	k_EOperator_Float_GTE,
	k_EOperator_Float_Not_GTE,
	k_EOperator_Subkey_Contains, 
	k_EOperator_Subkey_Not_Contains,
	k_EItemCriteriaOperator_Count
} EItemCriteriaOperator;

typedef enum EItemQuality
{
	k_EItemQuality_Normal, 
	k_EItemQuality_Genuine,
	k_EItemQuality_Unused3,
	k_EItemQuality_Vintage,
	k_EItemQuality_Unused5,
	k_EItemQuality_Unusual,
	k_EItemQuality_Common,
	k_EItemQuality_Community,
	k_EItemQuality_Valve,
	k_EItemQuality_SelfMade,
	k_EItemQuality_Unused10,
	k_EItemQuality_Strange,
	k_EItemQuality_Unk12,
	k_EItemQuality_Haunted,

	k_EItemQuality_Max = 0xFF,
} EItemQuality;

typedef enum EItemOrigin
{
	k_EItemOrigin_Dropped, 
	k_EItemOrigin_Achievement,
	k_EItemOrigin_Purchased,
	k_EItemOrigin_Traded,
	k_EItemOrigin_Crafted,
	k_EItemOrigin_Store,
	k_EItemOrigin_Gifted,
	k_EItemOrigin_Support,
	k_EItemOrigin_Crate,
	k_EItemOrigin_Earned,
	k_EItemOrigin_ThirdParty,
	k_EItemOrigin_Wrapped,
	k_EItemOrigin_Halloween,
	k_EItemOrigin_Steam,
	k_EItemOrigin_Foreign,
	k_EItemOrigin_CD,
	k_EItemOrigin_Reward,
	k_EItemOrigin_Preview,
	k_EItemOrigin_Workshop,
	k_EItemOrigin_Awarded,
	k_EItemOrigin_Max = 0xFF,
} EItemOrigin;

typedef enum EItemRequestResult
{
	k_EItemRequestResultOK = 0,
	k_EItemRequestResultDenied,
	k_EItemRequestResultServerError,
	k_EItemRequestResultTimeout,
	k_EItemRequestResultInvalid,
	k_EItemRequestResultNoMatch,
	k_EItemRequestResultUnknownError,
	k_EItemRequestResultNotLoggedOn
} EItemRequestResult;


#pragma pack( push, 8 )
struct UserItemCount_t
{
    enum { k_iCallback = k_iSteamUserItemsCallbacks };
    CGameID m_gameID;
    EItemRequestResult m_eResult;
    uint32 m_unCount;
};

//         Item ID         |    Unknown  |   Unknown
// c5 31 b4 00 00 00 00 00 | 00 00 00 00 | 00 00 00 00
// ce b7 15 02 00 00 00 00 | 00 00 00 00 | 00 00 00 00
struct UserItemMoved_t
{
	enum { k_iCallback = k_iSteamUserItemsCallbacks + 1 };

	ItemID m_itemID;
	uint32 Unk0;
	uint32 Unk1;
};
//        Item ID          |    Unknown  |   Unknown 
// c8 a0 15 02 00 00 00 00 | 00 00 00 00 | 00 00 00 00
struct UserItemDeleted_t
{
	enum { k_iCallback = k_iSteamUserItemsCallbacks + 2 };

	ItemID m_itemID;
	uint32 Unk0;
	uint32 Unk1;
};

//         Item ID         |    CGameID (440) ?
// 86 e7 43 02 00 00 00 00 | b8 01 00 00 00 00 00 00
// 37 da 43 02 00 00 00 00 | b8 01 00 00 00 00 00 00
// 4c cb 43 02 00 00 00 00 | b8 01 00 00 00 00 00 00
// 1f bf 43 02 00 0000 00  | b8 01 00 00 00 00 00 00
// thanks to Didrole for the sample data
struct UserItemGranted_t
{
	enum { k_iCallback = k_iSteamUserItemsCallbacks + 3 };

	ItemID m_itemID;
	CGameID m_gameID;
};

struct UserItemGetBlob_t
{
	enum { k_iCallback = k_iSteamUserItemsCallbacks + 4 };

	ItemID m_itemID;
	EItemRequestResult m_eResult;
	uint8 itemBlob[1024];
};

struct UserItemSetBlob_t
{
	enum { k_iCallback = k_iSteamUserItemsCallbacks + 5 };

	ItemID m_itemID;
	EItemRequestResult m_eResult;
	uint32 Unk;
};

#pragma pack(pop)

#endif // USERITEMSCOMMON_H
