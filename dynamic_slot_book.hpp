#ifndef DYNAMIC_SLOT_BOOK_H
#define DYNAMIC_SLOT_BOOK_H

#include "slot_book.hpp"

const unsigned int DEFAULT_MIN_PAGES = 1;

class DynamicSlotBook : public SlotBook
{

public:

	DynamicSlotBook( unsigned int width, unsigned int height, unsigned int min_pages = DEFAULT_MIN_PAGES );

	// Item management.
    bool initialize( void );
	bool insert_item( Item* item );
	void remove_item( Item* item );

private:

	// Page management.
	unsigned int get_min_pages( void ) const;
	void set_min_pages( unsigned int min_pages );

private:

	unsigned int min_pages_;

};

#endif // DYNAMIC_SLOT_BOOK_H