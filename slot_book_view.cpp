#include "slot_book_view.hpp"

SlotBookView::SlotBookView( SlotArrayInterface* slot_array, unsigned int page_width, unsigned int page_height )
	: SlotGridView( page_width, page_height ),
	  slot_array_( slot_array ),
	  active_page_( 0 )
{
}

SlotBookView::~SlotBookView( void )
{
}

/*
 * Sets the page that is being focused on and updates the listener.
 * This function assumes that the listener is set up to handle indices
 * in [0, page_size - 1] and will update them as such to represent
 * the active page.
 */
bool SlotBookView::set_active_page( unsigned int page )
{
	unsigned int page_size = get_grid_size();
	unsigned int active_page_start = get_active_page() * page_size;
	unsigned int total_slots = slot_array_->get_size();
	for (unsigned int i = active_page_start; i < active_page_start + page_size; ++i) {
		const Slot* slot;
		if (i < total_slots) {
			slot = slot_array_->get_slot( i );
		}
		else {
			slot = nullptr;
		}
		if (!on_slot_updated( i, slot )) {
			return false;
		}
	}
    return true;
}

/*
 * Get the page that is being focused on.
 */
unsigned int SlotBookView::get_active_page( void ) const
{
	return active_page_;
}

/*
 * Get the total number of pages.
 * Even if a page is incomplete, it is reported as complete page.
 */
unsigned int SlotBookView::get_page_count( void ) const
{
	// Round up and minimize to one page.
    unsigned int slot_count = slot_array_->get_size();
	unsigned int page_size = get_grid_size();
    if (slot_count < page_size) {
        slot_count = page_size;
    }
	unsigned int pages = (slot_count + (page_size - 1)) / page_size;
	return pages;
}

/*
 * Handle slot updated event for page view.
 */
bool SlotBookView::on_slot_updated( unsigned int index, const Slot* slot)
{
	// Only update if index is on current page.
	unsigned int page_size = get_grid_size();
	unsigned int active_page_start = get_active_page() * page_size;
	unsigned int active_page_end = active_page_start + page_size;
	if ((index >= active_page_start) && (index < active_page_end)) {
		unsigned int grid_index = index - active_page_start;
		if (!SlotGridView::on_slot_updated( grid_index, slot )) {
			return false;
		}
	}
	return true;
}