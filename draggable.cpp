#include "draggable.h"

//=============================================================
// Constructor
//=============================================================
Draggable::Draggable( float x, float y ) : ConstrainedContainer( x, y )
{
	SetParent( nullptr );
	SetDragging( false );
}

Draggable::~Draggable( void )
{
	// Draggable destroyed.
}

void Draggable::Draw( DirectX *directX )
{
	Container::Draw( directX );
}

bool Draggable::MouseMoved( Mouse *mouse )
{
	if (isDragging_) {
		SetPosition( mouse->GetX() + offsetX_, mouse->GetY() + offsetY_ );
		if (HasParent()) {
			GetParent()->ClampChild( this );
		}
		return true;
	}

	return false;
}

bool Draggable::MouseClicked( Mouse *mouse )
{
	if (mouse->IsTouching( this )) {
		BeginDragging( mouse );
		return true;
	}

	return false;
}

bool Draggable::MouseReleased( Mouse *mouse )
{
	if (mouse->IsTouching( this )) {
		EndDragging();
		return true;
	}

	return false;
}

//=============================================================
// Purpose:	Returns proper X position based on whether it is
//			being dragged right now.
//=============================================================
float Draggable::GetX( void ) const
{
	return Component::GetX();
}

//=============================================================
// Purpose:	Returns proper Y position based on whether it is
//			being dragged right now.
//=============================================================
float Draggable::GetY( void ) const
{
	return Component::GetY();
}

//=============================================================
// Purpose:	Enables dragging and sets offset to mouse.
//=============================================================
void Draggable::BeginDragging( const Mouse* mouse )
{
	// Set offset to mouse.
	SetOffset( GetX() - mouse->GetX(), GetY() - mouse->GetY() );
	SetDragging( true );
	mouse_ = mouse;
}

//=============================================================
// Purpose: Sets current position and disables dragging.
//=============================================================
void Draggable::EndDragging( void )
{
	SetDragging( false );
}

const Container* Draggable::GetParent() const
{
	return parent_;
}

bool Draggable::HasParent() const
{
	return parent_ != nullptr;
}

void Draggable::SetParent( const Container* container )
{
	parent_ = container;
}

bool Draggable::IsDragging( void ) const
{
	return isDragging_;
}

void Draggable::SetOffset( float x, float y )
{
	offsetX_ = x;
	offsetY_ = y;
}

void Draggable::SetDragging( bool isDragging )
{
	isDragging_ = isDragging;
}