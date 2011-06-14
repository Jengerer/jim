#include "serialized_buffer.h"

SerializedBuffer::SerializedBuffer( void* buffer )
{
	start_ = buffer;
	buffer_ = buffer;
}

void SerializedBuffer::push( unsigned int steps )
{
	buffer_ = (char*)buffer_ + steps;
}

void SerializedBuffer::write( void *data, unsigned int size )
{
	memcpy( buffer_, data, size );
	buffer_ = (char*)buffer_ + size;
}