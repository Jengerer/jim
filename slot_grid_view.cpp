#include "slot_grid_view.h"

/*
 * Slot grid view constructor.
 */
SlotGridView::SlotGridView( const SlotArray* slots, unsigned int grid_width, unsigned int spacing ) : GridLayout( grid_width, spacing )
{
	add_slots( slots );
}

/*
 * Add slots from an array to the grid.
 */
void SlotGridView::add_slots( const SlotArray* slots )
{
	unsigned int end = slots->get_slot_count();
	for (unsigned int i = 0; i < end; ++i) {
		Slot* slot = slots->get_slot( i );
		add_slot( slot );
	}

	// Pack grid.
	pack();
}

SlotView* SlotGridView::get_touching_slot( Mouse* mouse ) const
{
	// Find view being touched.
	// TODO: Can this be done mathematically?
	for each (SlotView* view in slot_views_) {
		if (mouse->is_touching( view )) {
			return (view->is_enabled() ? view : nullptr);
		}
	}

	return nullptr;
}

/*
 * Enable all slot views.
 */
void SlotGridView::set_enabled( bool is_enabled ) const
{
	for each (SlotView* slot_view in slot_views_) {
		slot_view->set_enabled( is_enabled );
	}
}

/*
 * Disable slot views with an item in them.
 */
void SlotGridView::disable_full( void ) const
{
	for each (SlotView* slot_view in slot_views_) {
		if (slot_view->get_slot()->has_item()) {
			slot_view->set_enabled( false );
		}
	}
}

/*
 * Add a slot to the grid.
 */
void SlotGridView::add_slot( Slot* slot )
{
	SlotView* view = new SlotView( slot );
	slot_views_.push_back( view );
	add( view );
}