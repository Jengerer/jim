#pragma once

#include "exception.h"
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
	virtual void LoadInterfaces( void );
	virtual void CloseInterfaces( void );

	// Callback and message handling.
	bool		GetCallback( CallbackMsg_t* callback ) const;
	void		ReleaseCallback( void ) const;
	bool		HasMessage( uint32* size ) const;
	void		GetMessage( unsigned int* id, void* buffer, uint32 size, unsigned int* realSize ) const;
	void		SendMessage( unsigned int id, void* buffer, uint32 size ) const;

	// SO version handling.
	void		SetVersion( uint64 version );
	uint64		GetVersion() const;

	// Steam getters.
	uint64		GetSteamId( void ) const;

	// For sending protobuf headers.
	void		SetTargetId( uint64 targetId );
	void		GenerateProtobufHeader( CMsgProtoBufHeader *headerMsg ) const;

private:

	uint64		GetTargetId( void ) const;

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