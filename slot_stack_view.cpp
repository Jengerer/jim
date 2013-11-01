#include "slot_stack_view.hpp"

// Item selection parameters.
const unsigned int STACK_DISABLE_ALPHA = 0;
const unsigned int STACK_ENABLE_ALPHA = 200;

SlotStackView::SlotStackView( SlotArrayInterface* slot_array )
    : slot_array_( slot_array )
{
	set_enabled( false );
}

SlotStackView::~SlotStackView( void )
{
}

/*
 * Handle slot container update event.
 */
bool SlotStackView::on_slot_updated( unsigned int index, const Slot* slot )
{
    // Only update top slot.
    if (index == 0) {
        if (!update( slot )) {
            return false;
        }

		// Update alpha.
		update_alpha();
    }
    return true;
}

/*
 * Set drag offset from mouse.
 */
void SlotStackView::set_drag_offset( int x, int y )
{
	offset_x_ = x;
	offset_y_ = y;
}

/*
 * Toggle enable/disable of slot drag view.
 */
void SlotStackView::set_enabled( bool is_enabled )
{
	is_enabled_ = is_enabled;
	update_alpha();
}

/*
 * Return whether stack is enabled.
 */
bool SlotStackView::is_enabled( void ) const
{
	return is_enabled_;
}

/*
 * Update slot alpha based on activity.
 */
void SlotStackView::update_alpha( void )
{
	if (is_enabled_) {
		set_alpha( STACK_ENABLE_ALPHA );
	}
	else {
		set_alpha( STACK_DISABLE_ALPHA );
	}
}

/*
 * Update drag position based on mouse position.
 */
JUI::IOResult SlotStackView::on_mouse_moved( JUI::Mouse* mouse )
{
	// Only update if we're dragging.
	if (is_enabled_) {
		int x = mouse->get_x() + offset_x_;
		int y = mouse->get_y() + offset_y_;
		set_position( x, y );
		return JUI::IO_RESULT_HANDLED;
	}
	return JUI::IO_RESULT_UNHANDLED;
}

/*
 * Dummy mouse handler.
 */
JUI::IOResult SlotStackView::on_mouse_clicked( JUI::Mouse* mouse )
{
	return JUI::IO_RESULT_UNHANDLED;
}

/*
 * Dummy mouse handler.
 */
JUI::IOResult SlotStackView::on_mouse_released( JUI::Mouse* mouse )
{
	return JUI::IO_RESULT_UNHANDLED;
}
