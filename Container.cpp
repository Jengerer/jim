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

void Container::SetPosition( float x, float y )
{
	Component::SetPosition( x, y );
	UpdatePosition();
}

void Container::UpdatePosition()
{
	// Optionally implemented.
}

//=============================================================
// Purpose: Checks whether a component is visibly contained
//			by this container.
//=============================================================
bool Container::WithinBounds( Component *component ) const
{
	int top = GetY() - component->GetHeight();
	int left = GetX() - component->GetWidth();
	int right = GetX() + GetWidth();
	int bottom = GetY() + GetHeight();

	return (component->GetX() >= left) &&
		(component->GetX() <= right) &&
		(component->GetY() >= top) &&
		(component->GetY() <= bottom);
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
	int leftBound = GetX() + padding;
	int rightBound = GetX() + GetWidth() - component->GetWidth() - padding;
	int topBound = GetY() + padding;
	int bottomBound = GetY() + GetHeight() - component->GetHeight() - padding;

	// Clamp X position.
	if ( component->GetX() < leftBound ) {
		component->SetX( leftBound );
	}
	else if ( component->GetX() > rightBound ) {
		component->SetX( rightBound );
	}

	// Clamp Y position.
	if ( component->GetY() < topBound ) {
		component->SetY( topBound );
	}
	else if ( component->GetY() > bottomBound ) {
		component->SetY( bottomBound );
	}
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
void Container::RemoveAll()
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
		if (WithinBounds( component )) {
			component->OnDraw( directX );
		}
	}
}