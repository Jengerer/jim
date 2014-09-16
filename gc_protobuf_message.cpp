#include "gc_protobuf_message.hpp"

/* Constructor for protobuf message. */
GCProtobufMessage::GCProtobufMessage(
	uint32 message_id,
	uint64 target_id,
	uint64 steam_id )
{
	// Check if we need to set up protobuf header.
	if (target_id != 0) {
		protobuf_header_.set_client_steam_id( steam_id );
		protobuf_header_.set_job_id_target( target_id );
		protobuf_header_size = protobuf_header.ByteSize();
	}
	else {
		protobuf_header_size = 0;
	}
	header_.m_eMsg = message_id;
	header_.m_cubProtobufHeader = protobuf_header_size;
}

/* Get the header with protobuf header information. */
GCProtobufHeader_t* GCProtobufMessage::get_header( void )
{
	return &header_;
}

/* Get the protobuf header. */
CMsgProtoBufHeader* GCProtobufHeader::get_protobuf_header( void )
{
	return &protobuf_header_;
}

/* Initialize a message with current header information for payload. */
bool GCProtobufHeader::initialize_message( google::protobuf::Message* payload )
{
	// Calculate buffer size.
	unsigned int header_size;
	unsigned int protobuf_header_size;
	unsigned int payload_size = payload->ByteSize();
	unsigned int buffer_size = sizeof(GCProtobufHeader_t) + payload_size;

	// Check if we should attach protobuf header.
	// TODO: Maybe the protobuf size is 0 if empty?
	if (!message_buffer_.set_size( buffer_size )) {
		return false;
	}

	// Copy the headers over.
	char* buffer = message_buffer_.get_array();
	GCProtobufHeader_t* buffer_Header = static_cast<GCProtobufHeader_t*>(buffer);
	buffer_header->m_eMsg = message_id;
	buffer_header->m_cubProtobufHeader = protobuf_header_size;

	// Copy the payload over.
	char* payload_buffer = &buffer[header_size];
	if (!payload->SerializeToArray( payload_buffer, payload_size )) {
		return false;
	}
	return true;
}

