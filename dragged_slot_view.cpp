#include "dragged_slot_view.h"

DraggedSlotView::DraggedSlotView( Slot* slot ) : SlotView( slot )
{
	// DraggedSlotView created.
}

DraggedSlotView::~DraggedSlotView()
{
	delete GetSlot();
}

bool DraggedSlotView::on_mouse_clicked( Mouse* mouse )
{
	return mouse->IsTouching( this );
}
bool DraggedSlotView::on_mouse_released( Mouse* mouse )
{
	return mouse->IsTouching( this );
}

bool DraggedSlotView::on_mouse_moved( Mouse* mouse )
{
	SetPosition( mouse->GetX() + offsetX_, mouse->GetY() + offsetY_ );
	return true;
}

void DraggedSlotView::SetOffset( float offsetX, float offsetY )
{
	offsetX_ = offsetX;
	offsetY_ = offsetY;
}

void DraggedSlotView::DragSlot( Slot* slot )
{
	GetSlot()->SetItem( slot->GetItem() );
	slot->SetItem( nullptr );
}

Item* DraggedSlotView::ReleaseSlot()
{
	Slot* slot = GetSlot();
	Item* item = slot->GetItem();
	slot->SetItem( nullptr );
	return item;
}