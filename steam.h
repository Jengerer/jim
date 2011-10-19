#pragma once

#include <stdexcept>
#include "steam/SteamclientAPI.h"
#include "protobuf/steammessages.pb.h"
#include "protobuf/base_gcmessages.pb.h"

// Header for receiving protobuf messages.
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

	Steam( void );
	virtual ~Steam( void );

	// Initializing and closing.
	virtual void load_interfaces( void );
	virtual void close_interfaces( void );

	// Callback and message handling.
	bool		get_callback( CallbackMsg_t* callback ) const;
	void		release_callback( void ) const;
	bool		has_message( uint32* size ) const;
	void		get_message( unsigned int* id, void* buffer, uint32 size, unsigned int* realSize ) const;
	void		send_message( unsigned int id, void* buffer, uint32 size ) const;

	// SO version handling.
	void		set_version( uint64 version );
	uint64		get_version() const;

	// Steam getters.
	uint64		get_steam_id( void ) const;

	// For sending protobuf headers.
	void		set_target_id( uint64 targetId );
	void		generate_protobuf_header( CMsgProtoBufHeader *headerMsg ) const;

private:

	uint64		get_target_id( void ) const;

private:

	// Steam handles.
	HSteamUser					hUser_;
	HSteamPipe					hPipe_;

	// Steam interfaces.
	ISteamClient008*			steamClient_;
	ISteamUser012*				steamUser_;
	ISteamGameCoordinator001*	gameCoordinator_;

	// Steam interface target.
	uint64						targetId_;
	uint64						version_;

	// For function loading.
	HMODULE						clientDll_;

};