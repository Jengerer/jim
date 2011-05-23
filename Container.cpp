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
	return true; // TODO: HOLY SHIT UNDO THIS WHEN YOU FIGURE OUT COMPONENTS.
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
	int finalX = component->GetX();
	if ( finalX < leftBound ) {
		finalX = leftBound;
	}
	else if ( finalX > rightBound ) {
		finalX = rightBound;
	}

	// Clamp Y position.
	int finalY = component->GetY();
	if ( finalY < topBound ) {
		finalY = topBound;
	}
	else if ( finalY > bottomBound ) {
		finalY = bottomBound;
	}

	component->SetPosition( finalX, finalY );
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
	D3DXMATRIX matWorld, matComponent;
	directX->GetWorldTransform( &matWorld );
	D3DXMatrixTranslation( &matComponent, GetX(), GetY(), 0.0f );
	directX->SetWorldTransform( &(matComponent) );

	// Draw all children.
	for (int i = 0; i < componentStack_.size(); i++) {
		Component *component = componentStack_.at( i );
		if (IsVisible( component )) {
			component->OnDraw( directX );
		}
	}

	directX->SetWorldTransform( &matWorld );
}