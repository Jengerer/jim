#include "slot.hpp"

/*
 * Slot constructor.
 */
Slot::Slot( void )
{
	set_item( nullptr );
	set_enabled( true );
}

/*
 * Return whether there's an item in this slot.
 */
bool Slot::has_item( void ) const
{
	return item_ != nullptr;
}

/*
 * Get the item contained in this slot.
 */
Item* Slot::get_item( void ) const
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
 * Return whether this slot is enabled.
 */
bool Slot::is_enabled( void ) const
{
    return is_enabled_;
}

/*
 * Set whether this slot is enabled.
 */
void Slot::set_enabled( bool is_enabled )
{
    is_enabled_ = is_enabled;
}
