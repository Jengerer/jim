#include "slot.h"

//=============================================================
// Constructor
//=============================================================
Slot::Slot( int index )
{
	SetItem( nullptr );
	SetIndex( index );
}

bool Slot::HasItem() const
{
	return item_ != nullptr;
}

Item* Slot::GetItem() const
{
	return item_;
}

void Slot::SetItem( Item* item )
{
	item_ = item;
}

unsigned int Slot::GetIndex() const
{
	return index_;
}

void Slot::SetIndex( unsigned int index )
{
	index_ = index;
}