#include "dynamic_slot_book.hpp"

/*
 * Dynamic slot book constructor.
 */
DynamicSlotBook::DynamicSlotBook( unsigned int page_size )
	: SlotBook( page_size )
{
}

/*
 * Returns whether this dyna