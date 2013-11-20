#include "slot_stack_view.hpp"

// Item selection parameters.
const unsigned int STACK_DRAGGING_ALPHA = 150;
const unsigned int STACK_NORMAL_ALPHA = 255;

SlotStackView::SlotStackView( SlotArrayInterface* slot_array )
    : slot_array_( slot_array ),
	  is_dragging_( false )
{
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
 * Begin dragging the slot view.
 */
void SlotStackView::begin_dragging( void )
{
	is_dragging_ = true;
	save_position();
	set_alpha( STACK_DRAGGING_ALPHA );
}

/*
 * End dragging the slot view.
 */
void SlotStackView::end_dragging( void )
{
	is_dragging_ = false;
	restore_position();
	set_alpha( STACK_NORMAL_ALPHA );
}

/*
 * Return whether stack is enabled.
 */
bool SlotStackView::is_dragging( void ) const
{
	return is_dragging_;
}

/*
 * Update drag position based on mouse position.
 */
JUI::IOResult SlotStackView::on_mouse_moved( JUI::Mouse* mouse )
{
	// Only update if we're dragging.
	if (is_dragging_) {
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
	// Handle drag start.
	if (mouse->is_touching( this )) {
		offset_x_ = get_x() - mouse->get_x();
		offset_y_ = get_y() - mouse->get_y();
		set_drag_offset( offset_x_, offset_y_ );
		begin_dragging();
		return JUI::IO_RESULT_HANDLED;
	}
	return JUI::IO_RESULT_UNHANDLED;
}

/*
 * Dummy mouse handler.
 */
JUI::IOResult SlotStackView::on_mouse_released( JUI::Mouse* mouse )
{
	if (is_dragging_) {
		end_dragging();
		return JUI::IO_RESULT_HANDLED;
	}
	return JUI::IO_RESULT_UNHANDLED;
}

/*
 * Save position to return to after drag.
 */
void SlotStackView::save_position( void )
{
	return_x_ = get_x();
	return_y_ = get_y();
}

/*
 * Return stack view to pre-drag position.
 */
void SlotStackView::restore_position( void )
{
	set_position( return_x_, return_y_ );
}
