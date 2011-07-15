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

#ifndef ISTEAM2BRIDGE002_H
#define ISTEAM2BRIDGE002_H
#ifdef _WIN32
#pragma once
#endif

#include "SteamTypes.h"
#include "BridgeCommon.h"

abstract_class ISteam2Bridge002
{
public:
	virtual void SetSteam2Ticket( uint8 *pubTicket, int cubTicket ) = 0;

	virtual bool SetAccountName( const char *szName ) = 0;
	virtual bool SetPassword( const char *szPassword ) = 0;
	virtual bool SetAccountCreationTime( RTime32 creationTime ) = 0;

	virtual unknown_ret CreateProcess( void *lpVACBlob, uint32 cbVACBlob, const char *, char *, uint32, void *, char *, uint32 ) = 0;

	virtual EUniverse GetConnectedUniverse() = 0;
	virtual const char *GetIPCountry() = 0;

	virtual uint32 GetNumLicenses() = 0;

	virtual uint32 GetLicensePackageID( uint32 licenseId ) = 0;
	virtual RTime32 GetLicenseTimeCreated( uint32 licenseId ) = 0;
	virtual RTime32 GetLicenseTimeNextProcess( uint32 licenseId ) = 0;
	virtual uint32 GetLicenseMinuteLimit( uint32 licenseId ) = 0;
	virtual uint32 GetLicenseMinutesUsed( uint32 licenseId ) = 0;
	virtual EPaymentMethod GetLicensePaymentMethod( uint32 licenseId ) = 0;
	virtual ELicenseFlags GetLicenseFlags( uint32 licenseId ) = 0;
	virtual const char *GetLicensePurchaseCountryCode( uint32 licenseId ) = 0;

	virtual bool SetOfflineMode( bool offlineMode ) = 0;

	virtual uint64 GetCurrentSessionToken() = 0;

	virtual void SetCellID( CellID_t cellId ) = 0;

	virtual void SetSteam2FullASTicket( uint8 *pubTicket, int cubTicket ) = 0;

	virtual bool UpdateAppOwnershipTicket( AppId_t appId, bool ) = 0;

	virtual uint32 GetAppOwnershipTicketLength( AppId_t appId ) = 0;
	virtual uint32 GetAppOwnershipTicketData( AppId_t appId, void *lpTicketData, uint32 cubTicketData ) = 0;

	virtual bool GetAppDecryptionKey( AppId_t appId, void *lpDecryptionKey, uint32 cubDecryptionKey ) = 0;

	virtual const char* GetPlatformName( bool* ) = 0;
};

#endif // ISTEAM2BRIDGE002_H
