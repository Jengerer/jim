#include "Container.h"

Container::Container()
{
	// Container created.
}

Container::Container( float x, float y ) : Component( x, y )
{
	// Container created.
}

Container::~Container()
{
	// Empty retained trash items.
	emptyTrash();

	// Container destroyed.
	deque<Component*>::iterator i;
	while (!componentStack_.empty()) {
		Component* component = componentStack_.back();
		remove( component );
		delete component;
	}
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

void Container::trash( Component *component )
{
	remove( component );
	componentTrash_.push_back( component );
}

void Container::emptyTrash()
{
	// Delete all elements.
	while (!componentTrash_.empty()) {
		Component *component = componentTrash_.back();
		componentTrash_.pop_back();
		delete component;
	}
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