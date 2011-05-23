#include "Draggable.h"

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

//=============================================================
// Purpose: Updates position of component when mouse moves.
//=============================================================
bool Draggable::OnMouseMoved( Mouse *mouse )
{
	return IsDragging();
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
	if (IsDragging()) {
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
void Draggable::OnDrag( const Mouse* mouse )
{
	// Set offset to mouse.
	SetOffset( mouse->GetX() - GetX(), mouse->GetY() - GetY() );
	SetDragging( true );
	mouse_ = mouse;
}

//=============================================================
// Purpose: Sets current position and disables dragging.
//=============================================================
void Draggable::OnRelease( void )
{
	SetPosition( GetX(), GetY() );
	SetDragging( false );
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

void Draggable::SetOffset( float x, float y )
{
	offsetX_ = x;
	offsetY_ = y;
}

void Draggable::SetDragging( bool isDragging )
{
	isDragging_ = isDragging;
}