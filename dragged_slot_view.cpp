#include "dragged_slot_view.h"

DraggedSlotView::DraggedSlotView( Slot* slot ) : SlotView( slot )
{
	// DraggedSlotView created.
}

DraggedSlotView::~DraggedSlotView()
{
	delete get_slot();
}

bool DraggedSlotView::on_mouse_clicked( Mouse* mouse )
{
	return mouse->is_touching( this );
}
bool DraggedSlotView::on_mouse_released( Mouse* mouse )
{
	return mouse->is_touching( this );
}

bool DraggedSlotView::on_mouse_moved( Mouse* mouse )
{
	set_position( mouse->get_x() + offsetX_, mouse->get_y() + offsetY_ );
	return true;
}

void DraggedSlotView::SetOffset( float offsetX, float offsetY )
{
	offsetX_ = offsetX;
	offsetY_ = offsetY;
}

void DraggedSlotView::DragSlot( Slot* slot )
{
	get_slot()->set_item( slot->get_item() );
	slot->remove_item();
}

Item* DraggedSlotView::ReleaseSlot()
{
	Slot* slot = get_slot();
	Item* item = slot->get_item();
	slot->remove_item();
	return item;
}