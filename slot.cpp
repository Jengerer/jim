#include "slot.h"

//=============================================================
// Constructor
//=============================================================
Slot::Slot()
{
	set_item( nullptr );
}

bool Slot::has_item() const
{
	return item_ != nullptr;
}

Item* Slot::get_item() const
{
	return item_;
}

void Slot::set_item( Item* item )
{
	item_ = item;
}