#include "Draggable.h"

//=============================================================
// Constructor
//=============================================================
Draggable::Draggable( float x, float y ) : Container( x, y )
{
	SetParent( nullptr );
	isDragging_ = false;

	offsetX_ = 0.0f;
	offsetY_ = 0.0f;
}

Draggable::~Draggable( void )
{
	// Draggable destroyed.
}

//=============================================================
// Purpose: Updates position of component when mouse moves.
//=============================================================
bool Draggable::OnMouseMoved( Mouse *mouse )
{
	if (isDragging_) {
		UpdatePosition();
		return true;
	}

	return false;
}

bool Draggable::OnLeftClicked( Mouse *mouse )
{
	OnDrag( mouse );
	return true;
}

bool Draggable::OnLeftReleased( Mouse *mouse )
{
	OnRelease();
	return true;
}

//=============================================================
// Purpose:	Returns proper X position based on whether it is
//			being dragged right now.
//=============================================================
float Draggable::GetX( void ) const
{
	if (isDragging_) {
		float x = mouse_->GetX() - offsetX_;

		// Constrain position if we have a parent.
		if (parent_ != nullptr) {
			if ( x < parent_->GetX() ) {
				x = parent_->GetX();
			}
			else if ( x + GetWidth() > parent_->GetX() + parent_->GetWidth() ) {
				x = parent_->GetX() + parent_->GetWidth() - GetWidth();
			}
		}

		return x;
	}

	return Component::GetX();
}

//=============================================================
// Purpose:	Returns proper Y position based on whether it is
//			being dragged right now.
//=============================================================
float Draggable::GetY( void ) const
{
	if (isDragging_) {
		float y = mouse_->GetY() - offsetY_;

		// Constrain position if we have a parent.
		if (parent_ != nullptr) {
			if ( y < parent_->GetY() ) {
				y = parent_->GetY();
			}
			else if ( y + GetHeight() > parent_->GetY() + parent_->GetHeight() ) {
				y = parent_->GetY() + parent_->GetHeight() - GetHeight();
			}
		}

		return y;
	}

	return Component::GetY();
}

//=============================================================
// Purpose:	Enables dragging and sets offset to mouse.
//=============================================================
void Draggable::OnDrag( Mouse* mouse )
{
	// Set offset to mouse.
	offsetX_ = mouse->GetX() - GetX();
	offsetY_ = mouse->GetY() - GetY();
	isDragging_ = true;
	mouse_ = mouse;
}

//=============================================================
// Purpose: Sets current position and disables dragging.
//=============================================================
void Draggable::OnRelease( void )
{
	SetPosition( GetX(), GetY() );
	isDragging_ = false;
}

//=============================================================
// Purpose:	Sets the container the position is being
//			constrained by.
//=============================================================
void Draggable::SetParent( Container *container )
{
	parent_ = container;
}

bool Draggable::IsDragging( void ) const
{
	return isDragging_;
}