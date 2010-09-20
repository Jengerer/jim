#pragma once

class SerializedBuffer
{
public:
	SerializedBuffer(void* whichBuffer);
	
	template<class whichType>
	whichType* getValue();

	template<class whichType>
	void pushBuffer(unsigned int numSteps);

	// For byte specification.
	void pushBuffer(unsigned int numBytes);

private:
	void* m_vPointer;
};