#include "slot_grid_view.hpp"

// Layout constants.
const unsigned int SLOT_SPACING = 5;

SlotGridView::SlotGridView( unsigned int grid_width, unsigned int grid_height )
	: grid_width_( grid_width ),
	  grid_height_( grid_height )
{
}

SlotGridView::~SlotGridView( void )
{
}

/*
 * Add slots from an array to the grid.
 */
bool SlotGridView::initialize( void )
{
	// Reserve space for elements.
	unsigned int elements = grid_width_ * grid_height_;
	if (!reserve( elements ) || !slot_views_.reserve( elements )) {
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
		slot_views_.push( view );
        if (!view->initialize()) {
            return false;
        }
	}

	// Pack grid.
	pack( grid_width_, SLOT_SPACING );
    return true;
}

/*
 * Get the number of slots in a row.
 */
unsigned int SlotGridView::get_grid_width( void ) const
{
	return grid_width_;
}

/*
 * Get the number of slots in a column.
 */
unsigned int SlotGridView::get_grid_height( void ) const
{
	return grid_height_;
}

/*
 * Get total slots in grid view.
 */
unsigned int SlotGridView::get_grid_size( void ) const
{
	return grid_width_ * grid_height_;
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
bool SlotGridView::on_slot_updated( unsigned int index, const Slot* slot )
{
	// TODO: Use item decorator classes to draw different item types.
	SlotView* view = slot_views_.at( index );
	if (!view->update( slot )) {
		return false;
	}
    return true;
}