#ifndef MOUSE_HANDLER_H
#define MOUSE_HANDLER_H

#include "mouse.h"

class IMouseHandler
{

public:

	virtual bool on_mouse_clicked( Mouse *mouse ) = 0;
	virtual bool on_mouse_released( Mouse *mouse ) = 0;
	virtual bool on_mouse_moved( Mouse *mouse ) = 0;

};

#endif // MOUSE_HANDLER_H