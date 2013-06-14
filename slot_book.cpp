#include "slot_book.hpp"

SlotBook::SlotBook( unsigned int page_width, unsigned int page_height )
	: page_width_( page_width ),
	  page_height_( page_height )
{
}

SlotBook::~SlotBook( void )
{
}

/*
 * Get the number of slots in one row of the page.
 */
unsigned int SlotBook::get_page_width( void ) const
{
	return page_width_;
}

/*
 * Get the number of slots in one column.
 */
unsigned int SlotBook::get_page_height( void ) const
{
	return page_height_;
}

/*
 * Get number of slots in a single page.
 */
unsigned int SlotBook::get_page_size( void ) const
{
	return page_width_ * page_height_;
}

/*
 * Sets the page that is being focused on and updates the listener.
 * This function assumes that the listener is set up to handle indices
 * in [0, page_size - 1] and will update them as such to represent
 * the active page.
 */
bool SlotBook::set_active_page( unsigned int page )
{
	unsigned int page_size = get_page_size();
	for (unsigned int i = 0; i < page_size; ++i) {
		// Update the first slot on this page as index 0, second as 1, etc.
		unsigned int slot_index = i + (get_active_page() * page_size);

		// Update as empty slot if slot index is not valid.
		Slot* slot;
		if (slot_index < get_size()) {
			slot = slots_.at( slot_index );
		}
		else {
			slot = nullptr;
		}
		listener_->on_slot_updated( i, slot );
	}
}

/*
 * Get the page that is being focused on.
 */
unsigned int SlotBook::get_active_page( void ) const
{
	return active_page_;
}

/*
 * Get the total number of pages.
 * Even if a page is incomplete, it is reported as complete page.
 */
unsigned int SlotBook::get_page_count( void ) const
{
	// Round up: add page_size / 2 and round down.
	unsigned int page_size = get_page_size();
	unsigned int pages = (get_size() + (page_size / 2)) / page_size;
	return pages;
}