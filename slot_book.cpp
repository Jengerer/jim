#include "slot_book.hpp"

/*
 * Slot book constructor.
 */
SlotBook::SlotBook( unsigned int width, unsigned int height )
{
	set_page_width( width );
	set_page_height( height );
}

/*
 * Slot book deconstructor.
 */
SlotBook::~SlotBook( void )
{
	destroy_pages();
}

/*
 * Add a page to the book.
 * Returns pointer to slot array on success, nullptr otherwise.
 */
const SlotArray* SlotBook::add_page( void )
{
    // Create page.
    SlotArray* page;
    if (!JUTIL::BaseAllocator::allocate( &page )) {
        return nullptr;
    }
    unsigned int page_size = get_page_size();
    unsigned int slot_count = get_slot_count();
    page = new (page) SlotArray( page_size, slot_count );
	if (!page->initialize()) {
		JUTIL::BaseAllocator::destroy( page );
		return false;
	}

    // Add to pages.
	if (!pages_.push( page )) {
        JUTIL::BaseAllocator::destroy( page );
        return nullptr;
    }
	return page;
}

/*
 * Add pages to the book.
 * If this function fails mid-way through creation,
 * the pages that were successfully added remain.
 */
bool SlotBook::add_pages( unsigned int pages )
{
	for (unsigned int i = 0; i < pages; ++i) {
        if (!add_page()) {
            return false;
        }
	}
}

/*
 * Remove last page from book.
 */
void SlotBook::remove_page( void )
{
	if (get_page_count() != 0) {
		unsigned int end_index = get_page_count() - 1;
		SlotArray* target = pages_.get( end_index );
        JUTIL::BaseAllocator::destroy( target );
		pages_.erase( end_index );
	}
}

/*
 * Get array for page. Assumes valid index.
 */
const SlotArray* SlotBook::get_page( unsigned int page ) const
{
	return pages_.get( page );
}

/*
 * Get number of pages.
 */
unsigned int SlotBook::get_page_count( void ) const
{
	return pages_.get_length();
}

/*
 * Get page width.
 */
unsigned int SlotBook::get_page_width( void ) const
{
	return width_;
}

/*
 * Get page height.
 */
unsigned int SlotBook::get_page_height( void ) const
{
	return height_;
}

/*
 * Get number of slots per page.
 */
unsigned int SlotBook::get_page_size( void ) const
{
	return get_page_width() * get_page_height();
}

/*
 * Get number of slots.
 */
unsigned int SlotBook::get_slot_count( void ) const
{
	return get_page_count() * get_page_size();
}

/*
 * Get slot at index. Assumes valid index.
 */
Slot* SlotBook::get_slot( unsigned int index ) const
{
	// Get page.
	unsigned int page = index / get_page_size();
	unsigned int index_page = index % get_page_size();
	return get_page( page )->get_slot( index_page );
}

/*
 * Set page width.
 */
void SlotBook::set_page_width( unsigned int width )
{
	width_ = width;
}

/*
 * Set page height.
 */
void SlotBook::set_page_height( unsigned int height )
{
	height_ = height;
}

/*
 * Destroy all pages.
 */
void SlotBook::destroy_pages( void )
{
    size_t i;
    size_t length = pages_.get_length();
    for (i = 0; i < length; ++i) {
        SlotArray* page = pages_.get( i );
        JUTIL::BaseAllocator::destroy( page );
    }
    pages_.clear();
}