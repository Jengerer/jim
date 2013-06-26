#ifndef DYNAMIC_SLOT_BOOK_HPP
#define DYNAMIC_SLOT_BOOK_HPP

#include "slot_book.hpp"

class DynamicSlotBook : public SlotBook
{

public:

	DynamicSlotBook( unsigned int page_size );

	// Dynamic slot management.
	bool has_empty_slot( void ) const;
	unsigned int get_empty_slot_index( void ) const;
	bool compress_slots( void );

};

#endif // DYNAMIC_SLOT_BOOK_HPP