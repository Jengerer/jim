#include "gc_protobuf_message.hpp"
#include <jui/application/error_stack.hpp>

/* Constructor for outbound protobuf message. */
GCProtobufMessage::GCProtobufMessage( uint32 message_id ) : message_id_( message_id )
{
}

/* Constructor for protobuf message. */
GCProtobufMessage::GCProtobufMessage(
	uint32 message_id,
	uint64 target_id,
	uint64 steam_id ) : message_id_( message_id )
{
	// Check if we need to set up protobuf header.
	if (target_id != 0) {
		protobuf_header_.set_client_steam_id( steam_id );
		protobuf_header_.set_job_id_target( target_id );
	}
}

/* Get the ID for this message. */
uint32 GCProtobufMessage::get_message_id( void ) const
{
	return message_id_;
}

/* Get the protobuf header. */
CMsgProtoBufHeader* GCProtobufMessage::get_protobuf_header( void )
{
	return &protobuf_header_;
}

/* Initialize a message from a received protobuf message buffer. */
bool GCProtobufMessage::initialize_inbound_message( JUTIL::ArrayBuilder<uint8>* input_buffer )
{
	// Error logging.
	JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

	// Take the buffer from the input.
	unsigned int size = input_buffer->get_size();
	uint8* buffer = input_buffer->release();
	message_buffer_.set_array( buffer, size );

	// Get information from the header.
	GCProtobufMessageHeader* header = reinterpret_cast<GCProtobufMessageHeader*>( buffer );
	char* next_buffer = reinterpret_cast<char*>( header + 1 );
	
	// Get the protobuf header if one exists.
	uint32 protobuf_header_size = header->protobuf_header_size;
	if (protobuf_header_size != 0) {
		if (!protobuf_header_.ParseFromArray( next_buffer, protobuf_header_size )) {
			stack->log( "Failed to parse protobuf header from message." );
			return false;
		}
	}
	return true;
}

/* Initialize an outbound message with current header information for payload. */
bool GCProtobufMessage::initialize_outbound_message( google::protobuf::Message* payload )
{
	// Error logging.
	JUI::ErrorStack *stack = JUI::ErrorStack::get_instance();

	// Calculate buffer size.
	unsigned int protobuf_header_size = protobuf_header_.ByteSize();
	unsigned int header_size = sizeof(GCProtobufMessageHeader) + protobuf_header_size;
	unsigned int payload_size = payload->ByteSize();
	unsigned int buffer_size = header_size + payload_size;

	// Check if we should attach protobuf header.
	if (!message_buffer_.set_size( buffer_size )) {
		stack->log( "Failed to allocate buffer for protobuf message." );
		return false;
	}

	// Copy the headers over.
	uint8* buffer = message_buffer_.get_array();
	GCProtobufMessageHeader* buffer_header = reinterpret_cast<GCProtobufMessageHeader*>(buffer);
	buffer_header->message_id = message_id_;
	buffer_header->protobuf_header_size = protobuf_header_size;

	// Copy the payload over.
	uint8* payload_buffer = &buffer[header_size];
	if (!payload->SerializeToArray( payload_buffer, payload_size )) {
		stack->log( "Failed to write payload to buffer." );
		return false;
	}
	return true;
}

/* Get the prepared buffer to send. */
const void* GCProtobufMessage::get_message_buffer( void ) const
{
	return message_buffer_.get_array();
}

/* Get the size of the prepared message. */
unsigned int GCProtobufMessage::get_message_size( void ) const
{
	return message_buffer_.get_size();
}

/* Get the payload from the buffer. */
void GCProtobufMessage::get_payload( const void** payload_out, unsigned int* size_out ) const
{
	// Get the memory past the header size.
	unsigned int payload_offset = get_header_size();
	unsigned int payload_size = message_buffer_.get_size() - payload_offset;
	*payload_out = reinterpret_cast<const void*>(message_buffer_.get_array() + payload_offset);
	*size_out = payload_size;
}

/* Get the header size. */
unsigned int GCProtobufMessage::get_header_size( void ) const
{
	return sizeof(GCProtobufMessageHeader) + protobuf_header_.ByteSize();
}
