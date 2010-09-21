#include "SerializedBuffer.h"

SerializedBuffer::SerializedBuffer(void* whichBuffer)
{
	m_vPointer = whichBuffer;
}

template<class whichType>
void SerializedBuffer::pushBuffer(unsigned int numSteps)
{
	m_vPointer = ((char*)m_vPointer + numBytes*whichType);
}

void SerializedBuffer::pushBuffer(unsigned int numBytes)
{
	m_vPointer = ((char*)m_vPointer + numBytes);
}