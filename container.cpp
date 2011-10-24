#include "container.h"
#include <algorithm>

//=============================================================
// Constructor
//=============================================================
Container::Container( float localX, float localY ) : Component( localX, localY )
{
	// Container created.
}

//=============================================================
// Destructor: removes and destroys all children.
//=============================================================
Container::~Container( void )
{
	// Container destroyed.
	std::vector<Component*>::iterator i;
	for (i = components_.begin(); i != components_.end(); i = components_.erase( i )) {
		delete *i;
	}
}

void Container::add( Component* component )
{
	components_.push_back( component );
}

//=============================================================
// Purpose: Removes a child from the container, but does
//			NOT destroy it.
//=============================================================
void Container::remove( Component* component )
{
	std::vector<Component*>::iterator i = find( components_.begin(), components_.end(), component );
	if (i != components_.end()) {
		components_.erase( i );
	}
}

//=============================================================
// Purpose: Draws all children.
//=============================================================
void Container::draw( Graphics2D* graphics )
{
	// Draw all children.
	for (auto i = components_.begin(), end = components_.end(); i != end; ++i) {
		Component* child = *i;
		if (IsVisible( child )) {
			child->draw( graphics );
		}
	}
}

//=============================================================
// Purpose: Sets alpha for all children.
//=============================================================
void Container::set_alpha( int alpha )
{
	Component::set_alpha( alpha );
	for (auto i = components_.begin(), end = components_.end(); i != end; ++i) {
		Component* child = *i;
		child->set_alpha( alpha );
	}
}

//=============================================================
// Purpose: Checks whether a component is bounded by this
//			container.
//=============================================================
bool Container::WithinBounds( Component *component ) const
{
	float childX = component->get_x();
	float childY = component->get_y();
	float parentX = get_x();
	float parentY = get_y();
	return (childX > parentX - component->get_width()) && 
		(childX < parentX + get_width()) && 
		(childY > parentY - component->get_height()) && 
		(childY < parentY + get_height());
}

//=============================================================
// Purpose: Returns true if the child is visible in the
//			given context and if it should be drawn.
//=============================================================
bool Container::IsVisible( Component *component ) const
{
	return WithinBounds( component );
}

//=============================================================
// Purpose: Constrains a component's position based on the
//			size and position of this container.
//=============================================================
void Container::clamp_child( Component *component, float padding ) const
{
	float childX = component->get_x();
	float childY = component->get_y();
	float parentX = get_x();
	float parentY = get_y();
	
	// Generate bounds.
	float leftBound = parentX + padding;
	float topBound = parentY + padding;

	// Clamp X position.
	if (childX < leftBound) {
		childX = leftBound;
	}
	else {
		float rightBound = parentX + get_width() - component->get_width() - padding;
		if (childX > rightBound) {
			childX = rightBound;
		}
	}

	// Clamp Y position.
	if (childY < topBound) {
		childY = topBound;
	}
	else {
		float bottomBound = parentY + get_height() - component->get_height() - padding;
		if (childY > bottomBound) {
			childY = bottomBound;
		}
	}

	component->set_position( childX, childY );
}