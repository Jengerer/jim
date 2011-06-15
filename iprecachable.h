#ifndef PRECACHABLE_H
#define PRECACHABLE_H

#include "directx.h"

class IPrecachable
{

public:

	virtual void Precache( DirectX *directX ) = 0;
	virtual void Release( void ) = 0;

};

#endif // PRECACHABLE