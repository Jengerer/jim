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

void SlotGridView::add_slots( const SlotVector* slots )
{
	add_slots( slots, 0, slots->get_slot_count() );
}

void SlotGridView::add_slots( const SlotVector* slots, unsigned int startIndex, unsigned int endIndex )
{
	for (unsigned int i = startIndex; i < endIndex; ++i) {
		Slot* slot = slots->get_slot_by_index( i );
		AddSlot( slot );
	}

	pack();
}

SlotView* SlotGridView::get_touching_slot( Mouse* mouse ) const
{
	vector<SlotView*>::const_iterator i, end;
	for (i = slotViews_.begin(), end = slotViews_.end(); i != end; ++i) {
		SlotView* slotView = *i;
		if (mouse->is_touching( slotView )) {
			return slotView;
		}
	}

	return nullptr;
}