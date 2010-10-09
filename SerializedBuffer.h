#pragma once

class SerializedBuffer
{
public:
	SerializedBuffer( void* buffer );
	
	template<class T>
	T* get()
	{
		T* value = (T*)buffer_;
		push( sizeof(T) );
		return value;
	}

	void* here() {
		return buffer_;
	}

	template<class T>
	void push( unsigned int steps );

	// For byte specification.
	void push( unsigned int bytes );

private:
	void* buffer_;
};