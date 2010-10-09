#include "SerializedBuffer.h"

SerializedBuffer::SerializedBuffer(void* buffer)
{
	buffer_ = buffer;
}

template<class T>
void SerializedBuffer::push( unsigned int steps )
{
	unsigned int numBytes = steps*sizeof(T);
	buffer_ = ((char*)buffer_ + numBytes);
}

void SerializedBuffer::push( unsigned int bytes )
{
	push<char>( bytes );
}