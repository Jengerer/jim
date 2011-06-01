#include "Container.h"

//=============================================================
// Constructor
//=============================================================
Container::Container( float x, float y ) : Component( x, y )
{
	// Container created.
}

//=============================================================
// Destructor: removes and destroys all children.
//=============================================================
Container::~Container()
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
	UpdatePosition();
}

void Container::UpdatePosition( void )
{
	// Move children by local X.
	deque<Component*>::iterator i, end;
	for (i = componentStack_.begin(), end = componentStack_.end(); i != end; ++i) {
		Component *component = *i;
		component->SetGlobalPosition( GetGlobalX() + component->GetLocalX(), GetGlobalY() + component->GetLocalY() );
	}
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
	float localX = component->GetLocalX();
	float localY = component->GetLocalY();
	return (localX >= -component->GetWidth()) && 
		(localY >= -component->GetHeight()) && 
		(localX <= GetWidth()) && 
		(localY <= GetHeight());
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
void Container::ClampChild( Component *component, int padding ) const
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
}

void Container::Add( Component* component )
{
	componentStack_.push_back( component );
}

void Container::AddBottom( Component* component )
{
	componentStack_.push_front( component );
}

//=============================================================
// Purpose: Removes a child from the container, but does
//			NOT destroy it.
//=============================================================
void Container::Remove( Component* component )
{
	deque<Component*>::iterator i = componentStack_.begin();
	while ( i != componentStack_.end() ) {
		Component* current = *i;
		if (current == component) {
			componentStack_.erase( i );
			break;
		}

		i++;
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