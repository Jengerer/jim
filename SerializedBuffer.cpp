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

template<class whichType>
whichType* getValue()
{
	whichType* returnValue = (whichType*)m_vPointer;
	pushBuffer(sizeof(whichType));
	return returnValue;
}