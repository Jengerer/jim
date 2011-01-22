#include "Component.h"

Component::Component()
{
	// Component created.
}

Component::Component( float x, float y )
{
	// Set defaults.
	setPosition( x, y );
}

Component::~Component()
{
	// Component is destroyed.
}

bool Component::mouseEvent( Mouse *mouse, EMouseEvent eventType )
{
	// Generate mouse event function.
	if (mouse->isTouching( this )) {
		// Call the appropriate function.
		if (eventType == MOUSE_EVENT_CLICK) {
			return mouseClicked( mouse );
		}
		else if (eventType == MOUSE_EVENT_RELEASE) {
			return mouseReleased( mouse );
		}
		else if (eventType == MOUSE_EVENT_MOVE) {
			return mouseMoved( mouse );
		}
		else {
			throw Exception( "Unhandled mouse event." );
		}
	}
}

bool Component::mouseClicked( Mouse *mouse )
{
	// Default handling.
	return false;
}

bool Component::mouseReleased( Mouse *mouse )
{
	// Default handling.
	return false;
}

bool Component::mouseMoved( Mouse *mouse )
{
	// Default handling.
	return false;
}

void Component::setX( float x ) {
	x_ = x;
}

void Component::setY( float y ) {
	y_ = y;
}

void Component::setPosition( float x, float y ) {
	setX( x );
	setY( y );
}

int Component::getWidth() const {
	return width_;
}

int Component::getHeight() const {
	return height_;
}

void Component::setSize( int width, int height ) {
	width_ = width;
	height_ = height;
}

float Component::getX() const {
	return x_;
}

float Component::getY() const {
	return y_;
}