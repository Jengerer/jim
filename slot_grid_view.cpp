#include "slot_grid_view.hpp"

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

SlotView* SlotGridView::get_touching_slot( JUI::Mouse* mouse ) const
{
	// Find view being touched.
	// TODO: This can be done with math.
    size_t i;
    size_t length = slot_views_.get_length();
    for (i = 0; i < length; ++i) {
        SlotView* view = slot_views_.get( i );
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
    size_t i;
    size_t length = slot_views_.get_length();
    for (i = 0; i < length; ++i) {
        SlotView* slot_view = slot_views_.get( i );
		slot_view->set_enabled( is_enabled );
	}
}

/*
 * Disable slot views with an item in them.
 */
void SlotGridView::disable_full( void ) const
{
	size_t i;
    size_t length = slot_views_.get_length();
    for (i = 0; i < length; ++i) {
        SlotView* slot_view = slot_views_.get( i );
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
	slot_views_.push( view );
	add( view );
}