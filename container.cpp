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
	deque<Component*>::iterator i;
	while (!componentStack_.empty()) {
		Component* component = componentStack_.back();
		Remove( component );
		delete component;
	}
}

void Container::Pack( void )
{
	// Optionally implemented.
}

void Container::SetGlobalPosition( float globalX, float globalY )
{
	Component::SetGlobalPosition( globalX, globalY );
	UpdateChildren();
}

void Container::UpdateChildren( void )
{
	// Move children by local position.
	deque<Component*>::iterator i, end;
	for (i = componentStack_.begin(), end = componentStack_.end(); i != end; ++i) {
		UpdateChild( *i );
	}
}

void Container::UpdateChild( Component *child ) const
{
	child->SetGlobalPosition(
		GetGlobalX() + child->GetLocalX(),
		GetGlobalY() + child->GetLocalY() );
}

void Container::SetAlpha( int alpha )
{
	Component::SetAlpha( alpha );
	deque<Component*>::iterator i;
	for (i = componentStack_.begin(); i != componentStack_.end(); i++) {
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
	float childX = component->GetGlobalX();
	float childY = component->GetGlobalY();
	float parentX = GetGlobalX();
	float parentY = GetGlobalY();
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
	int localX = component->GetLocalX();
	int localY = component->GetLocalY();

	// Clamp X position.
	if ( localX < padding ) {
		localX = padding;
	}
	else {
		float rightBound = GetWidth() - component->GetWidth() - padding;
		if ( localX > rightBound ) {
			localX = rightBound;
		}
	}

	// Clamp Y position.
	if ( localY < padding ) {
		localY = padding;
	}
	else {
		int bottomBound = GetHeight() - component->GetHeight() - padding;
		if ( localY > bottomBound ) {
			localY = bottomBound;
		}
	}

	component->SetLocalPosition( localX, localY );
	UpdateChild( component );
}

void Container::Add( Component* component )
{
	componentStack_.push_back( component );
}

//=============================================================
// Purpose: Removes a child from the container, but does
//			NOT destroy it.
//=============================================================
void Container::Remove( Component* component )
{
	deque<Component*>::iterator i = find( componentStack_.begin(), componentStack_.end(), component );
	if (i != componentStack_.end()) {
		componentStack_.erase( i );
	}
}

// TODO: Maybe make it recurse on child containers.
void Container::RemoveAll( void )
{
	componentStack_.clear();
}

deque<Component*>* Container::GetChildren()
{
	return &componentStack_;
}

//=============================================================
// Purpose: Draws all children.
//=============================================================
void Container::OnDraw( DirectX *directX )
{
	// Draw all children.
	for (int i = 0; i < componentStack_.size(); i++) {
		Component *component = componentStack_.at( i );
		if (IsVisible( component )) {
			component->OnDraw( directX );
		}
	}
}