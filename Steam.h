#pragma once

#include <vector>

#include "steam/SteamclientAPI.h"

#include "Item.h"
#include "Exception.h"

using namespace std;

#pragma pack(push, 1)

struct GCProtobufHeader_t
{
	uint32 m_EMsg;
	uint32 m_cubProtobufHeader;
};

#pragma pack(pop)

class Steam
{
public:
	Steam();
	virtual ~Steam();

	// Initializing and closing.
	virtual void openInterfaces();
	virtual void closeInterfaces();

	// Callback and message handling.
	bool getCallback( CallbackMsg_t* callback );
	void releaseCallback();
	bool hasMessage( uint32* size );
	void getMessage( unsigned int* id, void* buffer, uint32 size, unsigned int* realSize );
	void sendMessage( unsigned int id, void* buffer, uint32 size );

	// Interface handling.void
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

	// For function loading.
	HMODULE						clientDll_;
};