#include "index_slot.h"

IndexSlot::IndexSlot( unsigned int index )
{
	set_index( index );
}

void IndexSlot::set_index( unsigned int index )
{
	index_ = index;
}

unsigned int IndexSlot::get_index( void ) const
{
	return index_;
}

void IndexSlot::set_item( Item* item )
{
	Slot::set_item( item );

	if (item != nullptr) {
		item->set_index( index_ );
	}
}