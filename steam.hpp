#pragma once

#include <stdexcept>
#include "steam/SteamclientAPI.h"
#include "protobuf/base_gcmessages.pb.h"
#include "gc_protobuf_message.hpp"

// Protobuf flag.
const unsigned int PROTOBUF_MESSAGE_FLAG = 0x80000000;

// Log messages to file.
// #define LOG_STEAM_MESSAGES

class Steam
{

public:

	Steam( void );
	virtual ~Steam( void );

	// Initializing and closing.
	virtual bool load_interfaces( void );
	virtual void close_interfaces( void );

	// Callback and message handling.
	bool get_callback( CallbackMsg_t* callback ) const;
	void release_callback( void ) const;
	bool has_message( uint32* size ) const;
	bool get_message( unsigned int* id, void* buffer, uint32 size, unsigned int* real_size ) const;
	bool send_message( unsigned int id, const void* buffer, uint32 size ) const;
	bool send_protobuf_message( const GCProtobufMessage* message ) const;

	// Steam getters.
	uint64 get_steam_id( void ) const;

private:

	uint64 get_target_id( void ) const;

protected:

#if defined(LOG_STEAM_MESSAGES)
	// File for dumping data.
	FILE* log_;
	
	// Dump raw byte information to file.
	void log_bytes( const void *buffer, unsigned int size ) const;
#endif

private:

	// Steam handles.
	HSteamUser user_;
	HSteamPipe pipe_;

	// Steam interfaces.
	ISteamClient008* steam_client_;
	ISteamUser012* steam_user_;
	ISteamGameCoordinator001* game_coordinator_;

	// For function loading.
	HMODULE client_dll_;

};