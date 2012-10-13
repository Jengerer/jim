#include "draggable.hpp"

/*
 * Generic draggable object constructor.
 */
Draggable::Draggable( float x, float y ) : JUI::ConstrainedContainer( x, y )
{
	set_parent( nullptr );
	set_dragging( false );
}

/*
 * Generic draggable object destructor.
 */
Draggable::~Draggable( void )
{
}

/*
 * Handle mouse movement event; use offset from mouse if dragging.
 */
JUI::IOResult Draggable::on_mouse_moved( JUI::Mouse* mouse )
{
	// Move if dragging.
	if (is_dragging_) {
		set_position( mouse->get_x() + offset_x_, mouse->get_y() + offset_y_ );

		// Clamp if we have bounds.
		if (has_parent()) {
			get_parent()->clamp_child( this );
		}
		return JUI::IO_RESULT_HANDLED;
	}

	return JUI::IO_RESULT_UNHANDLED;
}

/*
 * Handle click event to start dragging.
 */
JUI::IOResult Draggable::on_mouse_clicked( JUI::Mouse* mouse )
{
	// Start dragging if clicked.
	if (mouse->is_touching( this )) {
		begin_dragging( mouse );
		return JUI::IO_RESULT_HANDLED;
	}

	return JUI::IO_RESULT_UNHANDLED;
}

/*
 * Handle release event to stop dragging.
 */
JUI::IOResult Draggable::on_mouse_released( JUI::Mouse* mouse )
{
	// Release dragging.
	if (is_dragging()) {
		end_dragging();
		return JUI::IO_RESULT_HANDLED;
	}

	return JUI::IO_RESULT_UNHANDLED;
}

/*
 * Enable dragging and track offset to mouse.
 */
void Draggable::begin_dragging( const JUI::Mouse* mouse )
{
	// Set offset to mouse.
	set_offset( get_x() - mouse->get_x(), get_y() - mouse->get_y() );
	set_dragging( true );
	mouse_ = mouse;
}

/*
 * Sets current position after disabling drag.
 */
void Draggable::end_dragging( void )
{
	set_dragging( false );
}

/*
 * Get parent container that we're bounded by, if any.
 */
const JUI::Container* Draggable::get_parent( void ) const
{
	return parent_;
}

/*
 * Return whether we have a parent container that we are bound to.
 */
bool Draggable::has_parent( void ) const
{
	return parent_ != nullptr;
}

/*
 * Set parent bounding container.
 */
void Draggable::set_parent( const Container* container )
{
	parent_ = container;
}

/*
 * Return whether we are being dragged.
 */
bool Draggable::is_dragging( void ) const
{
	return is_dragging_;
}

/*
 * Set offset from mouse at the point of dragging.
 */
void Draggable::set_offset( float x, float y )
{
	offset_x_ = x;
	offset_y_ = y;
}

/*
 * Set whether we are being dragged.
 */
void Draggable::set_dragging( bool is_dragging )
{
	is_dragging_ = is_dragging;
}