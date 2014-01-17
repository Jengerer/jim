#pragma once

#include <memory.h>

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

	void write( const void *data, unsigned int size );

	void* start() {
		return start_;
	}

	void* here() {
		return buffer_;
	}

	void push( unsigned int steps = 1 );

private:
	void* buffer_;
	void* start_;
};