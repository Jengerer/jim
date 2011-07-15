#include "slot.h"

//=============================================================
// Constructor
//=============================================================
Slot::Slot()
{
	SetItem( nullptr );
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