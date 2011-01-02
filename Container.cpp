#include "Container.h"

Container::Container( float x, float y ) : Component( x, y )
{
	// Container created.
}

Container::~Container()
{
	// Container destroyed.
	deque<Component*>::iterator i;
	while (!componentStack_.empty()) {
		Component* component = componentStack_.back();
		remove( component );
		delete component;
	}
}

bool Container::mouseEvent( Mouse *mouse, EMouseEvent eventType)
{
	// Replicate parent handling.
	if (Component::mouseEvent( mouse, eventType )) {
		// Now notify top-most children of the events.
		for (int i = componentStack_.size() - 1; i >= 0; i--) {
			Component* child = componentStack_[i];
			if (child->mouseEvent( mouse, eventType )) {
				return true;
			}
		}
	}

	return false;
}

void Container::setPosition( float x, float y )
{
	Component::setPosition( x, y );
	updatePosition();
}

void Container::updatePosition()
{
	// To be implemented.
}

void Container::add( Component* component )
{
	componentStack_.push_back( component );
}

void Container::remove( Component* component )
{
	deque<Component*>::iterator i = componentStack_.begin();
	while (i != componentStack_.end()) {
		Component* current = *i;
		if (current == component) {
			componentStack_.erase( i );
			break;
		}

		i++;
	}
}
