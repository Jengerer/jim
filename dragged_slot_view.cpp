#include "dragged_slot_view.h"

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
 * Return if mouse touching.
 */
bool DraggedSlotView::on_mouse_clicked( Mouse* mouse )
{
	return mouse->is_touching( this );
}

/*
 * Return if mouse touching.
 */
bool DraggedSlotView::on_mouse_released( Mouse* mouse )
{
	return mouse->is_touching( this );
}

/*
 * Update position offset to mouse.
 */
bool DraggedSlotView::on_mouse_moved( Mouse* mouse )
{
	set_position( mouse->get_x() + offset_x_, mouse->get_y() + offset_y_ );
	return true;
}

/*
 * Set offset to mouse.
 */
void DraggedSlotView::set_offset( float x, float y )
{
	offset_x_ = x;
	offset_y_ = y;
}