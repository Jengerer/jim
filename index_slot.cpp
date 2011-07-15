#include "index_slot.h"

IndexSlot::IndexSlot( unsigned int index )
{
	SetIndex( index );
}

void IndexSlot::SetIndex( unsigned int index )
{
	index_ = index;
}

unsigned int IndexSlot::GetIndex( void ) const
{
	return index_;
}

void IndexSlot::SetItem( Item* item )
{
	Slot::SetItem( item );

	if (item != nullptr) {
		item->SetIndex( index_ );
	}
}