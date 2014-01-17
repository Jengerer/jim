#include "slot_grid_view.hpp"

// Layout constants.
const unsigned int SLOT_SPACING = 5;

SlotGridView::SlotGridView( SlotArrayInterface* slot_array, unsigned int grid_width, unsigned int grid_height )
    : slot_array_( slot_array ),
	  grid_width_( grid_width ),
	  grid_height_( grid_height )
{
}

SlotGridView::~SlotGridView( void )
{
}

/*
 * Set the handler for slot grid view events.
 */
void SlotGridView::set_listener( SlotArrayViewListener* listener )
{
    listener_ = listener;
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
 * Handle mouse move event for slot hover.
 */
JUI::IOResult SlotGridView::on_mouse_moved( JUI::Mouse* mouse )
{
    // Check if hovering over a slot.
    unsigned int index;
    if (get_touching_index( mouse, &index )) {
        if (!listener_->on_slot_hovered( slot_array_, index )) {
            return JUI::IO_RESULT_ERROR;
        }
        return JUI::IO_RESULT_HANDLED;
    }
    return JUI::IO_RESULT_UNHANDLED;
}

/*
 * Handle mouse click events for slot click.
 */
JUI::IOResult SlotGridView::on_mouse_clicked( JUI::Mouse* mouse )
{
    // Check if clicking on a slot.
    unsigned int index;
    if (get_touching_index( mouse, &index )) {
		// Ignore ghost slot views.
		if (index < slot_array_->get_size()) {
			const Slot* slot = slot_array_->get_slot( index );

			// Don't register click for empty slots.
			if (slot->has_item()) {
				if (!listener_->on_slot_clicked( mouse, slot_array_, index )) {
					return JUI::IO_RESULT_ERROR;
				}
				return JUI::IO_RESULT_HANDLED;
			}
		}
    }
    return JUI::IO_RESULT_UNHANDLED;
}

/*
 * Handle mouse release event for releasing onto slot.
 */
JUI::IOResult SlotGridView::on_mouse_released( JUI::Mouse* mouse )
{
    // Check if releasing mouse over a slot.
    unsigned int index;
    if (get_touching_index( mouse, &index )) {
        if (!listener_->on_slot_released( slot_array_, index )) {
            return JUI::IO_RESULT_ERROR;
        }
        return JUI::IO_RESULT_HANDLED;
    }
    return JUI::IO_RESULT_UNHANDLED;
}

/*
 * Updates the slot view at the given index.
 */
void SlotGridView::on_slot_updated( unsigned int index, const Slot* slot )
{
	// TODO: Use item decorator classes to draw different item types.
	SlotView* view = slot_views_.at( index );
	view->update( slot );
}