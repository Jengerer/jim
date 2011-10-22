#include "slot_grid_view.h"

SlotGridView::SlotGridView( unsigned int gridWidth, unsigned int spacing ) : GridLayout( gridWidth, spacing )
{
	// SlotGridView created.
}

void SlotGridView::AddSlot( Slot* slot )
{
	SlotView* slotView = new SlotView( slot );
	slotViews_.push_back( slotView );
	add( slotView );
}

void SlotGridView::AddSlots( const SlotVector* slots )
{
	AddSlots( slots, 0, slots->GetSlotCount() );
}

void SlotGridView::AddSlots( const SlotVector* slots, unsigned int startIndex, unsigned int endIndex )
{
	for (unsigned int i = startIndex; i < endIndex; ++i) {
		Slot* slot = slots->GetSlotByIndex( i );
		AddSlot( slot );
	}

	Pack();
}

SlotView* SlotGridView::GetTouchingSlot( Mouse* mouse ) const
{
	vector<SlotView*>::const_iterator i, end;
	for (i = slotViews_.begin(), end = slotViews_.end(); i != end; ++i) {
		SlotView* slotView = *i;
		if (mouse->IsTouching( slotView )) {
			return slotView;
		}
	}

	return nullptr;
}