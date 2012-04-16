#include "slot_book.h"

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
 */
const SlotArray* SlotBook::add_page( void )
{
	SlotArray* page = new SlotArray( get_page_size(), get_slot_count() );
	pages_.push_back( page );
	return page;
}

/*
 * Add pages to the book.
 */
void SlotBook::add_pages( unsigned int pages )
{
	for (unsigned int i = 0; i < pages; ++i) {
		add_page();
	}
}

/*
 * Remove last page from book.
 */
void SlotBook::remove_page( void )
{
	if (get_page_count() != 0) {
		unsigned int end_index = get_page_count() - 1;
		SlotArray* target = pages_[ end_index ];
		pages_.erase( pages_.begin() + end_index );
		delete target;
	}
}

/*
 * Get array for page. Assumes valid index.
 */
const SlotArray* SlotBook::get_page( unsigned int page ) const
{
	return pages_[page];
}

/*
 * Get number of pages.
 */
unsigned int SlotBook::get_page_count( void ) const
{
	return pages_.size();
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
	std::vector<SlotArray*>::iterator i;
	for (i = pages_.begin(); i != pages_.end(); i = pages_.erase( i )) {
		delete *i;
	}
}