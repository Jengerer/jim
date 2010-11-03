#include "Component.h"

Component::Component( float x, float y ) {
	setPosition( x, y );

	// Add this component to the array.
	Mouse::createComponents();
	Mouse::addComponent( this );
}

Component::~Component() {
	// Component is destroyed.
	Mouse::removeComponent( this );
}

void Component::addMouseListener( MouseListener* mouseListener ) {
	mouseListeners_.push_back( mouseListener );
}

void Component::callMouseListener( EMouseEvent eventType ) {
	vector<MouseListener*>::iterator i;
	for (i = mouseListeners_.begin(); i != mouseListeners_.end(); i++) {
		MouseListener* mouseListener = *i;
		mouseListener->mouseClicked( NULL, this );
	}
}

void Component::setPosition( float x, float y ) {
	this->x = x;
	this->y = y;
}

float Component::getX() const {
	return x;
}

float Component::getY() const {
	return y;
}