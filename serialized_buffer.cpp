#include "serialized_buffer.hpp"

SerializedBuffer::SerializedBuffer( void* buffer ) :
	start_( buffer ),
	buffer_( buffer )
{
}

void SerializedBuffer::push( unsigned int steps )
{
	buffer_ = static_cast<char*>(buffer_) + steps;
}

void SerializedBuffer::write( const void *data, unsigned int size )
{
	memcpy( buffer_, data, size );
	buffer_ = static_cast<char*>(buffer_) + size;
}