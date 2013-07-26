#ifndef DYNAMIC_SLOT_BOOK_HPP
#define DYNAMIC_SLOT_BOOK_HPP

#include "slot_book.hpp"

class DynamicSlotBook : public SlotBook
{

public:

	DynamicSlotBook( unsigned int page_width, unsigned int page_height );

	// Dynamic slot management.
    bool push_item( Item* item );
	bool compress_slots( void );

};

#endif // DYNAMIC_SLOT_BOOK_HPP