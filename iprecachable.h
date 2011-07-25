#ifndef PRECACHABLE_H
#define PRECACHABLE_H

#include "graphics_2d.h"

class IPrecachable
{

public:

	virtual void Precache( Graphics2D* graphics ) = 0;
	virtual void Release( void ) = 0;

};

#endif // PRECACHABLE