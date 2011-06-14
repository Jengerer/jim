#ifndef MOUSE_HANDLER_H
#define MOUSE_HANDLER_H

#include "Mouse.h"

class IMouseHandler
{

public:

	virtual bool OnLeftClicked( Mouse *mouse ) = 0;
	virtual bool OnLeftReleased( Mouse *mouse ) = 0;
	virtual bool OnMouseMoved( Mouse *mouse ) = 0;

};

#endif // MOUSE_HANDLER_H