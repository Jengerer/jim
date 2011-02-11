#pragma once

#include <vector>

#include "Window.h"
#include "Component.h"

using namespace std;

enum EMouseEvent {
	MOUSE_EVENT_LEFT_CLICK,
	MOUSE_EVENT_LEFT_RELEASE,
	MOUSE_EVENT_RIGHT_CLICK,
	MOUSE_EVENT_RIGHT_RELEASE,
	MOUSE_EVENT_MOVE
};

class Component;
class Mouse
{
public:
	Mouse( Window* window );

	// Mouse control and settings.
	void setWindow( Window* window );
	void pollPosition();

	// Position functions.
	int getX() const;
	int getY() const;

	// Mouse state functions.
	void setLeftMouse( bool isClicked );
	void setRightMouse( bool isClicked );
	bool isLeftDown();
	bool isRightDown();

	// Call to components.
	bool isTouching( const Component* component );

private:
	Window*	window_;
	POINT	position_;
	bool	leftClicked_;
	bool	rightClicked_;
};

