#pragma once

/* Class for a protobuf message to be sent/received through the game coordinator. */
class GCProtobufMessage
{

public:

	GCProtobufMessage(
		uint32 message_id,
		uint64 target_id,
		uint64 steam_id );

	// Header retrieval functions.
	GCProtobufHeader_t* get_header( void );
	CMsgProtoBufHeader* get_protobuf_header( void );

	// Message initialization.
	bool initialize_message( google::protobuf::Message* payload );

	// Final message getters.
	void* get_message_buffer( void ) const;
	unsigned int get_message_size( void ) const;

private:

	// Header functions.
	unsigned int get_header_size( void ) const;

private:

	// Header structures.
	GCProtobufHeader_t header_;
	CMsgProtoBufHeader protobuf_header_;

	// Message payload.
	JUTIL::ArrayBuilder<char> message_buffer_;

};
