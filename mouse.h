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
	void set_window( Window* window );
	void poll();

	// State getter.
	void set_pressed( bool is_pressed );
	bool is_pressed() const;

	// Position functions.
	int get_x( void ) const;
	int get_y( void ) const;

	// Call to components.
	bool is_touching( const Component* component ) const;

private:

	bool is_pressed_;
	Window*	window_;
	POINT	position_;

};

#endif // MOUSE_H