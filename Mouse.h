#pragma once

#include <vector>

#include "Window.h"
#include "Component.h"

using namespace std;

enum EMouseEvent {
	MOUSE_EVENT_CLICK,
	MOUSE_EVENT_RELEASE,
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
	void leftMouseDown();
	void leftMouseUp();
	bool isLeftDown();

	// Call to components.
	void triggerEvent( Component* component, EMouseEvent eventType );
	bool isTouching( const Component* component );

private:
	Window*	window_;
	POINT	position_;
	bool	leftClicked_;
};

