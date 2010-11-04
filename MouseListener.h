#pragma once

#include "Mouse.h"
#include "Window.h"

enum EMouseEvent {
	MOUSE_EVENT_CLICK,
	MOUSE_EVENT_RELEASE,
	MOUSE_EVENT_MOVE,
};

class Mouse;
class Component;
class MouseListener
{
public:
	virtual ~MouseListener();

	// Mouse handling virtuals.
	virtual void mouseClicked( Mouse *mouse, Component *component ) = 0;
	virtual void mouseReleased( Mouse *mouse, Component *component ) = 0;
	virtual void mouseMoved( Mouse *mouse, Component *component ) = 0;
};