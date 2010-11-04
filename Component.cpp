#include "Component.h"

Component::Component( float x, float y )
{
	// Set position.
	setPosition( x, y );
}

Component::~Component()
{
	// Component is destroyed.
}

void Component::addMouseListener( MouseListener* mouseListener )
{
	mouseListeners_.push_back( mouseListener );
}

void Component::callMouseListener( Mouse* mouse, EMouseEvent eventType ) {
	vector<MouseListener*>::iterator i;
	for (i = mouseListeners_.begin(); i != mouseListeners_.end(); i++) {
		MouseListener* mouseListener = *i;
		if (eventType == MOUSE_EVENT_MOVE) {
			mouseListener->mouseMoved( mouse, this );
		}
		else if (eventType == MOUSE_EVENT_CLICK) {
			mouseListener->mouseClicked( mouse, this );
		}
		else if (eventType == MOUSE_EVENT_RELEASE) {
			mouseListener->mouseReleased( mouse, this );
		}
	}
}

void Component::setPosition( float x, float y ) {
	x_ = x;
	y_ = y;
}

float Component::getX() const {
	return x_;
}

float Component::getY() const {
	return y_;
}