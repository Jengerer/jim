#include "slot_grid_view.hpp"

// Layout constants.
const unsigned int SLOT_SPACING = 5;

SlotGridView::SlotGridView( void )
{
}

SlotGridView::~SlotGridView( void )
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
		new (view) SlotView();
		add( view );
        if (!view->initialize()) {
            return false;
        }
	}

	// Pack grid.
	pack( width, SLOT_SPACING );
    return true;
}

/*
 * Gets the slot view by index.
 */
const SlotView* SlotGridView::get_slot_view( unsigned int index ) const
{
	return slot_views_.at( index );
}

/*
 * Fills out the index of the slot view being touched by the mouse, if any.
 * Returns true if the mouse is touching a slot, false otherwise.
 */
bool SlotGridView::get_touching_index( const JUI::Mouse* mouse, unsigned int* index ) const
{
	// Find view being touched.
	// TODO: This can be done with math.
    size_t i;
    size_t length = slot_views_.get_length();
    for (i = 0; i < length; ++i) {
        SlotView* view = slot_views_.at( i );
		if (mouse->is_touching( view )) {
			*index = i;
			return true;
		}
	}

	return false;
}

/*
 * Updates the slot view at the given index.
 */
bool SlotGridView::on_slot_updated( unsigned int index, Slot* slot )
{
	// TODO: Use item decorator classes to draw different item types.
	SlotView* view = slot_views_.at( index );

	// Set item texture.
	Item* item;
	const JUI::Texture* item_texture;
	// Draw empty slot if beyond last item.
	if (slot != nullptr && (item = slot->get_item()) != nullptr) {
		item_texture = item->get_texture();
	}
	else {
		item_texture = nullptr;
	}
	view->set_item_texture( item_texture );
    return true;
}