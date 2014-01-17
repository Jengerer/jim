#include "slot_stack_view.hpp"

// Item selection parameters.
const float STACK_DRAGGING_ALPHA = 0.2f;
const float STACK_NORMAL_ALPHA = 0.0f;

SlotStackView::SlotStackView( SlotArrayInterface* slot_array )
    : slot_array_( slot_array ),
	  is_dragging_( false )
{
	set_alpha( STACK_NORMAL_ALPHA );
}

SlotStackView::~SlotStackView( void )
{
}

/*
 * Set the listener interface for array view.
 */
void SlotStackView::set_listener( SlotArrayViewListener* listener )
{
	listener_ = listener;
}

/*
 * Handle slot container update event.
 */
void SlotStackView::on_slot_updated( unsigned int index, const Slot* slot )
{
    // Only update top slot.
    if (index == 0) {
        update_item( slot );
    }
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
 * Should be called after offset is set.
 */
void SlotStackView::begin_dragging( void )
{
	is_dragging_ = true;
	set_alpha( STACK_DRAGGING_ALPHA );
}

/*
 * End dragging the slot view.
 */
void SlotStackView::end_dragging( void )
{
	is_dragging_ = false;
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
	// Handle drag start if non-empty.
	if (slot_array_->get_size() != 0) {
		if (mouse->is_touching( this )) {
			// Set offset and begin dragging.
			offset_x_ = get_x() - mouse->get_x();
			offset_y_ = get_y() - mouse->get_y();
			set_drag_offset( offset_x_, offset_y_ );
			begin_dragging();
			
			// Notify listener we've been clicked.
			if (!listener_->on_slot_clicked( mouse, slot_array_, 0 )) {
				return JUI::IO_RESULT_ERROR;
			}
			return JUI::IO_RESULT_HANDLED;
		}
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