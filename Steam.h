#pragma once

#include <vector>

#include "steam/SteamclientAPI.h"

#include "Item.h"
#include "Exception.h"

using namespace std;

class Steam
{
public:
	Steam();
	~Steam();

	/* Initializing and closing. */
	void openInterfaces();
	void closeInterfaces();

	/* Callback and message handling. */
	bool getCallback( CallbackMsg_t* callback );
	void releaseCallback();
	bool hasMessage( uint32* size );
	void getMessage( unsigned int* id, void* buffer, uint32 size, unsigned int* realSize );

	/* Interface handling. */
	void updateItem( Item* item );
	void deleteItem( uint64 uniqueId );

	/* Steam getters. */
	uint64 getSteamId() const;

private:
	/* Steam handlers. */
	HSteamUser					hUser_;
	HSteamPipe					HPipe_;

	/* Steam interfaces. */
	ISteamClient008*			steamClient_;
	ISteamUser012*				steamUser_;
	ISteamGameCoordinator001*	gameCoordinator_;
};