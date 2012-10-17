#include "dragged_slot_view.hpp"

/*
 * Dragged slot view constructor. Slot is mimicked; should
 * either be an existing slot, or one that will be destroyed.
 */
DraggedSlotView::DraggedSlotView( Slot* slot ) : SlotView( slot )
{
	// Nothing.
}

/*
 * Dragged slot view constructor.
 */
DraggedSlotView::~DraggedSlotView()
{
	// Nothing.
}

/*
 * Return handled if mouse touching.
 */
JUI::IOResult DraggedSlotView::on_mouse_clicked( JUI::Mouse* mouse )
{
    if (mouse->is_touching( this )) {
        return JUI::IO_RESULT_HANDLED;
    }

    return JUI::IO_RESULT_UNHANDLED;
}

/*
 * Return if mouse touching.
 */
JUI::IOResult DraggedSlotView::on_mouse_released( JUI::Mouse* mouse )
{
	if (mouse->is_touching( this )) {
        return JUI::IO_RESULT_HANDLED;
    }

    return JUI::IO_RESULT_UNHANDLED;
}

/*
 * Update position offset to mouse.
 */
JUI::IOResult DraggedSlotView::on_mouse_moved( JUI::Mouse* mouse )
{
	set_position( mouse->get_x() + offset_x_, mouse->get_y() + offset_y_ );
	return JUI::IO_RESULT_HANDLED;
}

/*
 * Set offset to mouse.
 */
void DraggedSlotView::set_offset( int x, int y )
{
	offset_x_ = x;
	offset_y_ = y;
}