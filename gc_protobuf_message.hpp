#pragma once

#include "steam/SteamTypes.h"
#include "protobuf/steammessages.pb.h"
#include <memory/array_builder.hpp>

// Header for receiving protobuf messages.
struct GCProtobufMessageHeader
{
	uint32 message_id;
	uint32 protobuf_header_size;
};

/* Class for a protobuf message to be sent/received through the game coordinator. */
class GCProtobufMessage
{

public:

	// Inbound message construction.
	GCProtobufMessage( uint32 message_id );

	// Outbound message construction.
	GCProtobufMessage(
		uint32 message_id,
		uint64 target_id,
		uint64 steam_id );

	// Header retrieval functions.
	uint32 get_message_id( void ) const;
	CMsgProtoBufHeader* get_protobuf_header( void );

	// Initialize a message from a received buffer.
	bool initialize_inbound_message( JUTIL::ArrayBuilder<uint8>* input_buffer );

	// Initialize message to send a payload.
	bool initialize_outbound_message( google::protobuf::Message* payload );

	// Final message getters.
	const void* get_message_buffer( void ) const;
	unsigned int get_message_size( void ) const;

	// Get the payload from the buffer.
	void get_payload( const void** payload_out, unsigned int *size_out ) const;

private:

	// Get header size.
	unsigned int get_header_size( void ) const;

private:

	// Header structures.
	uint32 message_id_;
	CMsgProtoBufHeader protobuf_header_;

	// Message payload.
	JUTIL::ArrayBuilder<uint8> message_buffer_;

};
