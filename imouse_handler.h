#ifndef MOUSE_HANDLER_H
#define MOUSE_HANDLER_H

#include "mouse.h"

class IMouseHandler
{

public:

	virtual bool MouseClicked( Mouse *mouse ) = 0;
	virtual bool MouseReleased( Mouse *mouse ) = 0;
	virtual bool MouseMoved( Mouse *mouse ) = 0;

};

#endif // MOUSE_HANDLER_H