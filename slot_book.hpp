#ifndef SLOT_BOOK_HPP
#define SLOT_BOOK_HPP

#include "slot_array.hpp"

/*
 * Class to control viewing a slot array a page at a time.
 */
class SlotBook : public SlotArray
{

public:

	SlotBook( unsigned int page_size );
	virtual ~SlotBook( void );

	// Page size functions.
	unsigned int get_page_size( void ) const;

	// Page switching functions.
	bool set_active_page( unsigned int page );
	unsigned int get_active_page( void ) const;
	unsigned int get_page_count( void ) const;

	// Paged item handling.
	Item* get_item_from_page( unsigned int page_index );

private:

	unsigned int active_page_;
	unsigned int page_size_;

};

#endif // SLOT_BOOK_HPP