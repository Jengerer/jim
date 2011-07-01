#include "draggable.h"

//=============================================================
// Constructor
//=============================================================
Draggable::Draggable( float x, float y ) : Container( x, y )
{
	Initialize();
}

Draggable::~Draggable( void )
{
	// Draggable destroyed.
}

void Draggable::Initialize( void )
{
	SetParent( nullptr );
	SetDragging( false );
}

void Draggable::OnDraw( DirectX *directX )
{
	Container::OnDraw( directX );
}

bool Draggable::OnMouseMoved( Mouse *mouse )
{
	if (isDragging_) {
		UpdateChildren();
		return true;
	}

	return false;
}

bool Draggable::OnLeftClicked( Mouse *mouse )
{
	if (mouse->IsTouching( this )) {
		OnDrag( mouse );
		return true;
	}

	return false;
}

bool Draggable::OnLeftReleased( Mouse *mouse )
{
	if (mouse->IsTouching( this )) {
		OnRelease();
		return true;
	}

	return false;
}

//=============================================================
// Purpose:	Returns proper X position based on whether it is
//			being dragged right now.
//=============================================================
float Draggable::GetGlobalX( void ) const
{
	if (isDragging_) {
		float x = mouse_->GetX() + offsetX_;

		// Constrain position if we have a parent.
		if (parent_ != nullptr) {
			float leftBound = parent_->GetGlobalX();
			if (x < leftBound) {
				return leftBound;
			}
			else {
				float rightBound = parent_->GetGlobalX() + parent_->GetWidth() - GetWidth();
				if (x > rightBound) {
					return rightBound;
				}
			}
		}

		return x;
	}

	return Component::GetGlobalX();
}

//=============================================================
// Purpose:	Returns proper Y position based on whether it is
//			being dragged right now.
//=============================================================
float Draggable::GetGlobalY( void ) const
{
	if (isDragging_) {
		float y = mouse_->GetY() + offsetY_;

		// Constrain position if we have a parent.
		if (parent_ != nullptr) {
			float topBound = parent_->GetGlobalY();
			if ( y < topBound ) {
				return topBound;
			}
			else {
				float bottomBound = parent_->GetGlobalY() + parent_->GetHeight() - GetHeight();
				if (y > bottomBound) {
					return bottomBound;
				}
			}
		}

		return y;
	}

	return Component::GetGlobalY();
}

//=============================================================
// Purpose:	Enables dragging and sets offset to mouse.
//=============================================================
void Draggable::OnDrag( const Mouse* mouse )
{
	// Set offset to mouse.
	SetOffset( GetGlobalX() - mouse->GetX(), GetGlobalY() - mouse->GetY() );
	SetDragging( true );
	mouse_ = mouse;
}

//=============================================================
// Purpose: Sets current position and disables dragging.
//=============================================================
void Draggable::OnRelease( void )
{
	SetGlobalPosition( GetGlobalX(), GetGlobalY() );
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