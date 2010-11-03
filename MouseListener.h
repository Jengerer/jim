#pragma once

#include "Mouse.h"
#include "Window.h"

enum EMouseEvent {
	MOUSE_EVENT_CLICK,
	MOUSE_EVENT_RELEASE,
	MOUSE_EVENT_MOVE,
};

class MouseListener
{
public:
	// Mouse handling virtuals.
	virtual void mouseClicked( Mouse *mouse ) = 0;
	virtual void mouseReleased( Mouse *mouse ) = 0;
	virtual void mouseMoved( Mouse *mouse ) = 0;
};