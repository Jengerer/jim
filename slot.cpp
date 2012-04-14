#include "slot.h"

/*
 * Slot constructor.
 */
Slot::Slot( void )
{
	remove_item();
}

/*
 * Return whether there's an item in this slot.
 */
bool Slot::has_item() const
{
	return item_ != nullptr;
}

/*
 * Get the item contained in this slot.
 */
Item* Slot::get_item() const
{
	return item_;
}

/*
 * Set item contained in slot.
 */
void Slot::set_item( Item* item )
{
	item_ = item;
}

/*
 * Remove item from slot.
 */
void Slot::remove_item( void )
{
	set_item( nullptr );
}