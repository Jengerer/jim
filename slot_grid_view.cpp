#include "slot_grid_view.hpp"

// Layout constants.
const unsigned int SLOT_SPACING = 5;

/*
 * Slot grid view constructor.
 */
SlotGridView::SlotGridView( void )
{
}

/*
 * Add slots from an array to the grid.
 */
bool SlotGridView::set_grid_size( unsigned int width, unsigned int height )
{
	// Reserve space for elements.
	unsigned int elements = width * height;
	if (!reserve( elements )) {
		return false;
	}

	// Create slot views.
	for (unsigned int i = 0; i < elements; ++i) {
		SlotView* view;
		if (!JUTIL::BaseAllocator::allocate( &view )) {
			return false;
		}
		add( view );
	}

	// Pack grid.
	pack( width, SLOT_SPACING );
    return true;
}

SlotView* SlotGridView::get_touching_slot( JUI::Mouse* mouse ) const
{
	// Find view being touched.
	// TODO: This can be done with math.
    size_t i;
    size_t length = slot_views_.get_length();
    for (i = 0; i < length; ++i) {
        SlotView* view = slot_views_.at( i );
		if (mouse->is_touching( view )) {
			return (view->is_enabled() ? view : nullptr);
		}
	}

	return nullptr;
}

/*
 * Return a slot view by its index.
 */
SlotView* SlotGridView::get_slot_view( unsigned int index ) const
{
	return slot_views_.at( index );
}

/*
 * Enable all slot views.
 */
void SlotGridView::set_enabled( bool is_enabled ) const
{
    size_t i;
    size_t length = slot_views_.get_length();
    for (i = 0; i < length; ++i) {
        SlotView* slot_view = slot_views_.at( i );
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
        SlotView* slot_view = slot_views_.at( i );
		if (slot_view->get_slot()->has_item()) {
			slot_view->set_enabled( false );
		}
	}
}