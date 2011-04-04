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

void Container::setPosition( float x, float y )
{
	Component::setPosition( x, y );
	updatePosition();
}

void Container::updatePosition()
{
	// Optionally implemented.
}

bool Container::withinBounds( Component *component )
{
	int top = getY() - component->getHeight();
	int left = getX() - component->getWidth();
	int right = getX() + getWidth();
	int bottom = getY() + getHeight();

	return (component->getX() >= left) &&
		(component->getX() <= right) &&
		(component->getY() >= top) &&
		(component->getY() <= bottom);
}

void Container::clampChild( Component *component, int padding )
{
	int leftBound = getX() + padding;
	int rightBound = getX() + getWidth() - component->getWidth() - padding;
	int topBound = getY() + padding;
	int bottomBound = getY() + getHeight() - component->getHeight() - padding;

	// Clamp X position.
	if (component->getX() < leftBound) {
		component->setX( leftBound );
	}
	else if (component->getX() > rightBound) {
		component->setX( rightBound );
	}

	// Clamp Y position.
	if (component->getY() < topBound) {
		component->setY( topBound );
	}
	else if (component->getY() > bottomBound) {
		component->setY( bottomBound );
	}
}

void Container::add( Component* component )
{
	componentStack_.push_back( component );
}

void Container::addBottom( Component* component )
{
	componentStack_.push_front( component );
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

void Container::removeAll()
{
	componentStack_.clear();
}

deque<Component*>* Container::getChildren()
{
	return &componentStack_;
}

void Container::draw( DirectX *directX )
{
	for (int i = 0; i < componentStack_.size(); i++) {
		Component *component = componentStack_.at( i );
		if (withinBounds( component )) {
			component->draw( directX );
		}
	}
}