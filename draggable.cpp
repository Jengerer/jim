#include "draggable.hpp"

//=============================================================
// Constructor
//=============================================================
Draggable::Draggable( float x, float y ) : JUI::ConstrainedContainer( x, y )
{
	set_parent( nullptr );
	SetDragging( false );
}

Draggable::~Draggable( void )
{
	// Draggable destroyed.
}

void Draggable::draw( JUI::Graphics2D* graphics )
{
	Container::draw( graphics );
}

bool Draggable::on_mouse_moved( JUI::Mouse* mouse )
{
	if (isDragging_) {
		set_position( mouse->get_x() + offsetX_, mouse->get_y() + offsetY_ );
		if (has_parent()) {
			get_parent()->clamp_child( this );
		}
		return true;
	}

	return false;
}

bool Draggable::on_mouse_clicked( JUI::Mouse* mouse )
{
	if (mouse->is_touching( this )) {
		begin_dragging( mouse );
		return true;
	}

	return false;
}

bool Draggable::on_mouse_released( JUI::Mouse* mouse )
{
	if (mouse->is_touching( this )) {
		end_dragging();
		return true;
	}

	return false;
}

//=============================================================
// Purpose:	Returns proper X position based on whether it is
//			being dragged right now.
//=============================================================
float Draggable::get_x( void ) const
{
	return Component::get_x();
}

//=============================================================
// Purpose:	Returns proper Y position based on whether it is
//			being dragged right now.
//=============================================================
float Draggable::get_y( void ) const
{
	return Component::get_y();
}

//=============================================================
// Purpose:	Enables dragging and sets offset to mouse.
//=============================================================
void Draggable::begin_dragging( const JUI::Mouse* mouse )
{
	// Set offset to mouse.
	SetOffset( get_x() - mouse->get_x(), get_y() - mouse->get_y() );
	SetDragging( true );
	mouse_ = mouse;
}

//=============================================================
// Purpose: Sets current position and disables dragging.
//=============================================================
void Draggable::end_dragging( void )
{
	SetDragging( false );
}

const Container* Draggable::get_parent() const
{
	return parent_;
}

bool Draggable::has_parent() const
{
	return parent_ != nullptr;
}

void Draggable::set_parent( const Container* container )
{
	parent_ = container;
}

bool Draggable::is_dragging( void ) const
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