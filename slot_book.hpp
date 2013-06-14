#ifndef SLOT_BOOK_HPP
#define SLOT_BOOK_HPP

#include "slot_array.hpp"

/*
 * Class to control viewing a slot array a page at a time.
 */
class SlotBook : public SlotArray
{

public:

	SlotBook( unsigned int page_width, unsigned int page_height );
	virtual ~SlotBook( void );

	// Page size functions.
	unsigned int get_page_width( void ) const;
	unsigned int get_page_height( void ) const;
	unsigned int get_page_size( void ) const;

	// Page switching functions.
	bool set_active_page( unsigned int page );
	unsigned int get_active_page( void ) const;
	unsigned int get_page_count( void ) const;

private:

	unsigned int active_page_;
	unsigned int page_width_;
	unsigned int page_height_;

};

#endif // SLOT_BOOK_HPP