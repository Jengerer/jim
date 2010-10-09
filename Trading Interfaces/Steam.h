#pragma once

#include <vector>

#include "steam/SteamclientAPI.h"
#include "Exception.h"

using namespace std;

class Steam
{
public:
	Steam();
	~Steam();

	// Initializing and closing.
	void openInterfaces();
	void closeInterfaces();

	// Callback and message handling.
	bool getCallback( CallbackMsg_t* callback );
	void releaseCallback();
	bool hasMessage( uint32* size );
	void getMessage( uint32* id, void* buffer, uint32 size, uint32* realSize );
	void sendMessage( uint32 id, void* message, uint32 size );

	// Friend interface handling.
	int getFriends();
	CSteamID getFriend( int index );
	bool getFriendGameInfo( CSteamID steamId, FriendGameInfo_t* friendGameInfo );
	const char* getPersonaName( CSteamID steamId );

	// Interface handling.
	void deleteItem( uint64 uniqueId );

	// Steam getters.
	uint64 getSteamId() const;

private:
	// Steam handles.
	HSteamUser					hUser_;
	HSteamPipe					hPipe_;

	// Steam interfaces.
	ISteamClient008*			steamClient_;
	ISteamUser012*				steamUser_;
	ISteamGameCoordinator001*	gameCoordinator_;
	ISteamFriends005*			steamFriends_;

	// For function loading.
	HMODULE						clientDll_;
};