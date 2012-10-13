#include "slot_grid_view.hpp"

/*
 * Slot grid view constructor.
 */
SlotGridView::SlotGridView( unsigned int grid_width, unsigned int spacing ) : GridLayout( grid_width, spacing )
{
}

/*
 * Add slots from an array to the grid.
 */
bool SlotGridView::add_slots( const SlotArray* slots )
{
	unsigned int end = slots->get_slot_count();
	for (unsigned int i = 0; i < end; ++i) {
		Slot* slot = slots->get_slot( i );
		if (!add_slot( slot )) {
			return false;
		}
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
bool SlotGridView::add_slot( Slot* slot )
{
	SlotView* view;
	if (!JUTIL::BaseAllocator::allocate( &view )) {
		return false;
	}
	view = new (view) SlotView( slot );

	// Add to container.
	if (!add( view )) {
		JUTIL::BaseAllocator::destroy( view );
		return false;
	}

	// Add to array.
	if (!slot_views_.push( view )) {
		return false;
	}
}