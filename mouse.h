#ifndef MOUSE_H
#define MOUSE_H

#include "window.h"
#include "component.h"

using namespace std;

enum EMouseEvent {
	MOUSE_EVENT_LEFT_CLICK,
	MOUSE_EVENT_LEFT_RELEASE,
	MOUSE_EVENT_RIGHT_CLICK,
	MOUSE_EVENT_RIGHT_RELEASE,
	MOUSE_EVENT_MOVE
};

enum EMouseCursor {
	MOUSE_CURSOR_ARROW,
	MOUSE_CURSOR_FINGER,
	MOUSE_CURSOR_HAND
};

class Mouse
{

public:

	Mouse( Window* window );

	// Mouse control and settings.
	void SetWindow( Window* window );
	void Poll( void );

	// Position functions.
	int GetX( void ) const;
	int GetY( void ) const;

	// Call to components.
	bool IsTouching( const Component* component ) const;

private:

	Window*	window_;
	POINT	position_;

};

#endif // MOUSE_H