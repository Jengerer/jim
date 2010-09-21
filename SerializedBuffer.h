#pragma once

class SerializedBuffer
{
public:
	SerializedBuffer(void* whichBuffer);
	
	template<class whichType>
	whichType* getValue()
	{
		whichType* returnValue = (whichType*)m_vPointer;
		pushBuffer(sizeof(whichType));
		return returnValue;
	}

	template<class whichType>
	void pushBuffer(unsigned int numSteps);

	// For byte specification.
	void pushBuffer(unsigned int numBytes);

private:
	void* m_vPointer;
};