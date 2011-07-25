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

void Container::Add( Component* component )
{
	components_.push_back( component );
}

//=============================================================
// Purpose: Removes a child from the container, but does
//			NOT destroy it.
//=============================================================
void Container::Remove( Component* component )
{
	vector<Component*>::iterator i = find( components_.begin(), components_.end(), component );
	if (i != components_.end()) {
		components_.erase( i );
	}
}

//=============================================================
// Purpose: Draws all children.
//=============================================================
void Container::Draw( Graphics2D* graphics )
{
	// Draw all children.
	std::vector<Component*>::iterator i, end;
	for (i = components_.begin(), end = components_.end(); i != end; ++i) {
		Component* child = *i;
		if (IsVisible( child )) {
			child->Draw( graphics );
		}
	}
}

void Container::SetAlpha( int alpha )
{
	Component::SetAlpha( alpha );
	vector<Component*>::iterator i, end;
	for (i = components_.begin(), end = components_.end(); i != end; ++i) {
		Component *component = *i;
		component->SetAlpha( alpha );
	}
}

//=============================================================
// Purpose: Checks whether a component is bounded by this
//			container.
//=============================================================
bool Container::WithinBounds( Component *component ) const
{
	float childX = component->GetX();
	float childY = component->GetY();
	float parentX = GetX();
	float parentY = GetY();
	return (childX > parentX - component->GetWidth()) && 
		(childX < parentX + GetWidth()) && 
		(childY > parentY - component->GetHeight()) && 
		(childY < parentY + GetHeight());
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
void Container::ClampChild( Component *component, float padding ) const
{
	float childX = component->GetX();
	float childY = component->GetY();
	float parentX = GetX();
	float parentY = GetY();
	
	// Generate bounds.
	float leftBound = parentX + padding;
	float topBound = parentY + padding;

	// Clamp X position.
	if (childX < leftBound) {
		childX = leftBound;
	}
	else {
		float rightBound = parentX + GetWidth() - component->GetWidth() - padding;
		if (childX > rightBound) {
			childX = rightBound;
		}
	}

	// Clamp Y position.
	if (childY < topBound) {
		childY = topBound;
	}
	else {
		int bottomBound = parentY + GetHeight() - component->GetHeight() - padding;
		if (childY > bottomBound) {
			childY = bottomBound;
		}
	}

	component->SetPosition( childX, childY );
}