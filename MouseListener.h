#pragma once

#include "Window.h"

enum EMouseEvent {
	MOUSE_EVENT_CLICK,
	MOUSE_EVENT_RELEASE,
	MOUSE_EVENT_MOVE,
};

class MouseListener
{
public:
	MouseListener( Window* window = 0 );

	// Mouse control and settings.
	void setWindow( Window* window );

	// Mouse state functions.
	void pollMouse();
	virtual void onMouseClick() = 0;
	virtual void onMouseRelease() = 0;
	virtual void onMouseMove() = 0;

	// Position functions.
	void getPosition( int* x, int* y ) const;
	void setPosition( int x, int y );
	int getMouseX() const;
	int getMouseY() const;

private:
	Window*		window_;
	POINT		position_;
};