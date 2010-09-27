#include "SerializedBuffer.h"

SerializedBuffer::SerializedBuffer(void* buffer)
{
	buffer_ = buffer;
}

template<class T>
void SerializedBuffer::push( unsigned int steps )
{
	buffer_ = ((char*)buffer_ + steps*sizeof(T));
}

void SerializedBuffer::push( unsigned int bytes )
{
	push<char>( bytes );
}