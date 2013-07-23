#include "slot_book.hpp"

SlotBook::SlotBook( unsigned int page_size )
	: page_size_( page_size )
{
}

SlotBook::~SlotBook( void )
{
}

/*
 * Get number of slots in a single page.
 */
unsigned int SlotBook::get_page_size( void ) const
{
	return page_size_;
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
			slot = &slots_.at( slot_index );
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

/*
 * Return the item from the current active page by index.
 */
Item* SlotBook::get_item_from_page( unsigned int page_index )
{
	unsigned int index = (page_size_ * active_page_) + page_index;
	return get_item( index );
}