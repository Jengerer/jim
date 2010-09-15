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

#ifndef ICLIENTFRIENDS_H
#define ICLIENTFRIENDS_H
#ifdef _WIN32
#pragma once
#endif

#include "SteamTypes.h"

#define CLIENTFRIENDS_INTERFACE_VERSION "CLIENTFRIENDS_INTERFACE_VERSION001"

class UNSAFE_INTERFACE IClientFriends
{
public:

	// returns the local players name - guaranteed to not be NULL.
	virtual const char *GetPersonaName() = 0;

	// sets the player name, stores it on the server and publishes the changes to all friends who are online
	virtual void SetPersonaName( const char *pchPersonaName ) = 0;

	virtual bool IsPersonaNameSet() = 0;

	// gets the friend status of the current user
	virtual EPersonaState GetPersonaState() = 0;
	// sets the status, communicates to server, tells all friends
	virtual void SetPersonaState( EPersonaState ePersonaState ) = 0;

	// friend iteration
	virtual int GetFriendCount( int iFriendFlags ) = 0;
	virtual CSteamID GetFriendByIndex( int iFriend, int iFriendFlags ) = 0;

	// gets the relationship to a user
	virtual EFriendRelationship GetFriendRelationship( CSteamID steamIDFriend ) = 0;
	// returns true if the specified user is considered a friend (can see our online status)
	virtual EPersonaState GetFriendPersonaState( CSteamID steamIDFriend ) = 0;
	// returns the name of a friend - guaranteed to not be NULL.
	virtual const char *GetFriendPersonaName( CSteamID steamIDFriend ) = 0;

	virtual void SetFriendAlias( CSteamID steamIDFriend, const char *pchAlias ) = 0;

	// gets the avatar of the current user, which is a handle to be used in IClientUtils::GetImageRGBA(), or 0 if none set
	virtual int GetFriendAvatar( CSteamID steamIDFriend, int a ) = 0;

	// steam registry, accessed by friend
	virtual void SetFriendRegValue( CSteamID steamIDFriend, const char *pchKey, const char *pchValue ) = 0;
	virtual const char *GetFriendRegValue( CSteamID steamIDFriend, const char *pchKey ) = 0;

	virtual bool DeleteFriendRegValue( CSteamID steamID, const char *pchKey ) = 0;

	virtual bool GetFriendGamePlayed( CSteamID steamID, FriendGameInfo_t * ) = 0;
	virtual const char *GetFriendGamePlayedExtraInfo( CSteamID steamID ) = 0;

	virtual CSteamID GetFriendGameServer( CSteamID steamID ) = 0;

	// accesses old friends names - returns an empty string when their are no more items in the history
	virtual const char *GetFriendPersonaNameHistory( CSteamID steamIDFriend, int iPersonaName ) = 0;

	virtual bool AddFriend( CSteamID steamID ) = 0;
	virtual bool RemoveFriend( CSteamID steamID ) = 0;
	virtual bool HasFriend( CSteamID steamID, int iFriendFlags ) = 0;

	// adds a friend by email address or account name - value returned in callback
	virtual HSteamCall AddFriendByName( const char *pchEmailOrAccountName ) = 0;

	virtual bool InviteFriendByEmail( const char *pchEmailAddress ) = 0;

	virtual bool RequestUserInformation( CSteamID steamID ) = 0;

	virtual bool SetIgnoreFriend( CSteamID steamID, bool bIgnore ) = 0;

	virtual bool ReportChatDeclined( CSteamID steamID ) = 0;

	// chat message iteration
	// returns the number of bytes in the message, filling pvData with as many of those bytes as possible
	// returns 0 if the steamID or iChatID are invalid
	virtual int GetChatMessage( CSteamID steamIDFriend, int iChatID, void *pvData, int cubData, EChatEntryType *peChatEntryType, CSteamID* pSteamIDChatter ) = 0;

	// generic friend->friend message sending, takes a sized buffer
	virtual bool SendMsgToFriend( CSteamID steamIDFriend, EChatEntryType eChatEntryType, const void *pvMsgBody, int cubMsgBody ) = 0;

	// returns the chatID that a chat should be resumed from when switching chat contexts
	virtual int GetChatIDOfChatHistoryStart( CSteamID steamIDFriend ) = 0;
	// sets where a chat with a user should resume
	virtual void SetChatHistoryStart( CSteamID steamIDFriend, int iChatID ) = 0;
	// clears the chat history - should be called when a chat dialog closes
	// the chat history can still be recovered by another context using SetChatHistoryStart() to reset the ChatIDOfChatHistoryStart
	virtual void ClearChatHistory( CSteamID steamIDFriend ) = 0;

	virtual int GetKnownClanCount() = 0;
	virtual CSteamID GetKnownClanByIndex( int iClan ) = 0;
	virtual int GetClanCount() = 0;
	virtual CSteamID GetClanByIndex( int iClan ) = 0;

	virtual const char *GetClanName( CSteamID steamID ) = 0;
	virtual bool GetClanActivityCounts( CSteamID steamID, int *pnOnline, int *pnInGame, int *pnChatting ) = 0;

	virtual bool IsClanPublic( CSteamID steamID ) = 0;
	virtual bool IsClanLarge( CSteamID steamID ) = 0;

	virtual bool InviteFriendToClan( CSteamID steamIDfriend, CSteamID steamIDclan ) = 0;
	virtual bool AcknowledgeInviteToClan( CSteamID steamID, bool bAcceptOrDenyClanInvite ) = 0;

	// iterators for any source
	virtual int GetFriendCountFromSource( CSteamID steamIDSource ) = 0;
	virtual CSteamID GetFriendFromSourceByIndex( CSteamID steamIDSource, int iFriend ) = 0;
	virtual bool IsUserInSource( CSteamID steamIDUser, CSteamID steamIDSource ) = 0;

	virtual int GetCoplayFriendCount() = 0;
	virtual CSteamID *GetCoplayFriend( int iCoplayEvent ) = 0;

	// most likely a RTime32
	virtual RTime32 *GetFriendCoplayTime( CSteamID steamID ) = 0;
	virtual CGameID GetFriendCoplayGame( CSteamID steamID ) = 0;

	virtual bool JoinChatRoom( CSteamID steamID ) = 0;
	virtual void LeaveChatRoom( CSteamID steamID ) = 0;

	virtual bool InviteUserToChatRoom( CSteamID steamIDfriend, CSteamID steamIDchat ) = 0;

	virtual bool SendChatMsg( CSteamID steamIDchat, EChatEntryType eChatEntryType, const void *pvMsgBody, int cubMsgBody ) = 0;

	virtual int GetChatRoomEntry( CSteamID steamIDchat, int iChatID, CSteamID *steamIDuser, void *pvData, int cubData, EChatEntryType *peChatEntryType ) = 0;

	virtual int GetChatIDOfChatRoomHistoryStart( CSteamID steamIDchat ) = 0;
	virtual void SetChatRoomHistoryStart( CSteamID steamIDchat, int iChatID ) = 0;

	virtual void ClearChatRoomHistory( CSteamID steamID ) = 0;

	virtual bool SerializeChatRoomDlg( CSteamID steamIDchat, void const* pvHistory, int cubHistory ) = 0;
	virtual int GetSizeOfSerializedChatRoomDlg( CSteamID steamIDchat ) = 0;
	virtual bool GetSerializedChatRoomDlg( CSteamID steamIDchat, void* pvHistory, int cubBuffer, int* pcubData ) = 0;
	virtual bool ClearSerializedChatRoomDlg( CSteamID steamIDchat ) = 0;

	virtual bool KickChatMember( CSteamID steamIDchat, CSteamID steamIDuser ) = 0;
	virtual bool BanChatMember( CSteamID steamIDchat, CSteamID steamIDuser ) = 0;
	virtual bool UnBanChatMember( CSteamID steamIDchat, CSteamID steamIDuser ) = 0;

	virtual bool SetChatRoomType( CSteamID steamIDchat, ELobbyType ) = 0;
	virtual bool GetChatRoomLockState( CSteamID steamIDchat, bool *pbLocked ) = 0;
	virtual bool GetChatRoomPermissions( CSteamID steamIDchat, uint32 * ) = 0;

	virtual unknown_ret SetChatRoomModerated( CSteamID steamIDchat, bool bModerated ) = 0;
	virtual bool ChatRoomModerated( CSteamID steamIDChat ) = 0;

	virtual bool NotifyChatRoomDlgsOfUIChange( CSteamID steamIDchat, bool, bool, bool, bool ) = 0;

	virtual bool TerminateChatRoom( CSteamID steamIDchat ) = 0;

	virtual int GetChatRoomCount() = 0;
	virtual CSteamID GetChatRoomByIndex( int iChatRoom ) = 0;

	virtual const char *GetChatRoomName( CSteamID steamIDchat ) = 0;

	virtual bool GetChatRoomMemberDetails( CSteamID steamIDchat, CSteamID steamIDuser, uint32* pChatMemberDetails, uint32* pChatMemberDetailsLocal ) = 0;

	virtual void CreateChatRoom( EChatRoomType eType, uint64 ulGameID, const char *pchName, ELobbyType eLobbyType, CSteamID steamIDClan, CSteamID steamIDFriendChat, CSteamID steamIDInvited, uint32 chatPermissionOfficer, uint32 chatPermissionMember, uint32 chatPermissionAll ) = 0;

	virtual const char *GetChatRoomMetadata( CSteamID steamIDchat, CSteamID steamIDuser, const char *pchKey ) = 0;
	virtual bool SetChatRoomMetadata( CSteamID steamIDChat, CSteamID steamIDMember, const char *pchKey, const char *pchValue ) = 0;

	virtual bool SetChatRoomPermissions( CSteamID steamIDchat, CSteamID steamIDmemeber, uint32 permissions, bool bMakeOwner ) = 0;

	virtual void VoiceCall( CSteamID steamIDlocal, CSteamID steamIDremote ) = 0;
	virtual void VoiceHangUp( HVoiceCall hVoiceCall ) = 0;

	virtual bool SetVoiceSpeakerVolume( float flVolume ) = 0;
	virtual bool SetVoiceMicrophoneVolume( float flVolume ) = 0;

	virtual void SetAutoAnswer( bool bAutoAnswer ) = 0;

	virtual void VoiceAnswer( HVoiceCall hVoiceCall ) = 0;

	virtual void VoicePutOnHold( HVoiceCall HVoiceCall, bool bOnLocalHold ) = 0;
	virtual bool VoiceIsLocalOnHold( HVoiceCall hVoiceCall ) = 0;
	virtual bool VoiceIsRemoteOnHold( HVoiceCall hVoiceCall ) = 0;

	virtual void SetDoNotDisturb( bool bDoNotDisturb ) = 0;

	virtual void EnableVoiceNotificationSounds( bool bEnable ) = 0;

	virtual void SetPushToTalkEnabled( bool bEnabled ) = 0;
	virtual bool IsPushToTalkEnabled() = 0;

	virtual void SetPushToTalkKey( int nKey ) = 0;
	virtual int GetPushToTalkKey() = 0;

	virtual bool IsPushToTalkKeyDown() = 0;

	virtual void EnableVoiceCalibration( bool bEnable ) = 0;
	virtual bool IsVoiceCalibrating() = 0;
	virtual float GetVoiceCalibrationSamplePeak() = 0;

	virtual void SetForceMicRecord( bool bForce ) = 0;
	virtual bool GetForceMicRecord() = 0;

	virtual void SetMicBoost( bool bBoost ) = 0;
	virtual bool GetMicBoost() = 0;

	virtual bool HasHardwareMicBoost() = 0;

	virtual const char *GetMicDeviceName() = 0;

	virtual void StartTalking( HVoiceCall hVoiceCall ) = 0;
	virtual void EndTalking( HVoiceCall hVoiceCall ) = 0;

	virtual bool VoiceIsValid( HVoiceCall hVoiceCall ) = 0;

	virtual void SetAutoReflectVoice( bool bAuto ) = 0;

	virtual ECallState GetCallState( HVoiceCall hVoiceCall ) = 0;

	virtual float GetVoiceMicrophoneVolume() = 0;
	virtual float GetVoiceSpeakerVolume() = 0;

	virtual float TimeSinceLastVoiceDataReceived( HVoiceCall hVoiceCall ) = 0;
	virtual float TimeSinceLastVoiceDataSend( HVoiceCall hVoiceCall ) = 0;

	virtual bool CanSend( HVoiceCall hVoiceCall ) = 0;
	virtual bool CanReceive( HVoiceCall hVoiceCall ) = 0;

	virtual float GetEstimatedBitsPerSecond( HVoiceCall hVoiceCall, bool bIncoming ) = 0;
	virtual float GetPeakSample( HVoiceCall hVoiceCall, bool bIncoming ) = 0;

	virtual void SendResumeRequest( HVoiceCall hVoiceCall ) = 0;

	virtual void OpenChatDialog( CSteamID steamID ) = 0;

	virtual void StartChatRoomVoiceSpeaking( CSteamID steamIDchat, CSteamID steamIDuser ) = 0;
	virtual void EndChatRoomVoiceSpeaking( CSteamID steamIDchat, CSteamID steamIDuser ) = 0;

	virtual RTime32 GetFriendLastLogonTime( CSteamID steamID ) = 0;
	virtual RTime32 GetFriendLastLogoffTime( CSteamID steamID ) = 0;

	virtual int GetChatRoomVoiceTotalSlotCount( CSteamID steamIDchat ) = 0;
	virtual int GetChatRoomVoiceUsedSlotCount( CSteamID steamIDchat ) = 0;
	virtual CSteamID GetChatRoomVoiceUsedSlot( CSteamID steamID, int iSlot ) = 0;
	virtual EChatRoomVoiceStatus GetChatRoomVoiceStatus( CSteamID steamIDchat, CSteamID steamIDuser ) = 0;

	virtual bool ChatRoomHasAvailableVoiceSlots( CSteamID steamID ) = 0;

	virtual bool IsChatRoomVoiceSpeaking( CSteamID steamIDchat, CSteamID steamIDuser ) = 0;

	virtual float GetChatRoomPeakSample( CSteamID steamIDchat, CSteamID steamIDuser, bool bIncoming ) = 0;

	virtual void ChatRoomVoiceRetryConnections( CSteamID steamIDchat ) = 0;

	virtual void SetPortTypes( uint32 unFlags ) = 0;

	virtual void ReinitAudio() = 0;

	virtual void SetInGameVoiceSpeaking( CSteamID steamIDuser, bool bIsSpeaking ) = 0;

	virtual void ActivateGameOverlay( const char *pchDialog ) = 0;
	virtual void ActivateGameOverlayToUser( const char *pchDialog, CSteamID steamID ) = 0;
	virtual void ActivateGameOverlayToWebPage( const char *pchUrl ) = 0;
	virtual void ActivateGameOverlayToStore( AppId_t nAppId ) = 0;

	virtual void NotifyGameOverlayStateChanged( bool bActive ) = 0;
	virtual void NotifyGameServerChangeRequested( const char *pchServerAddress, const char *pchPassword ) = 0;
	virtual bool NotifyLobbyJoinRequested( AppId_t nAppId, CSteamID steamIDlobby, CSteamID steamIDfriend ) = 0;

	virtual EClanRelationship GetClanRelationship( CSteamID steamIDclan ) = 0;

	virtual EClanRank GetFriendClanRank( CSteamID steamIDuser, CSteamID steamIDclan ) = 0;

	virtual bool VoiceIsAvailable() = 0;

	virtual void TestVoiceDisconnect( HVoiceCall hVoiceCall ) = 0;
	virtual void TestChatRoomPeerDisconnect( CSteamID steamIDchat, CSteamID steamIDuser ) = 0;
	virtual void TestVoicePacketLoss( float flPacketDropFraction ) = 0;

	virtual HVoiceCall FindFriendVoiceChatHandle( CSteamID steamID ) = 0;
};

#endif // ICLIENTFRIENDS_H
