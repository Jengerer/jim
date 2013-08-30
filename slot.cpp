#include "slot.hpp"

/*
 * Slot constructor.
 */
Slot::Slot( void )
{
	set_item( nullptr );
	set_enabled( true );
	set_selected( false );
}

/*
 * Return whether there's an item in this slot.
 * TODO: We usually get item pointer if there is an item,
 * so this function may not be needed, since we can just
 * get the pointer and check it.
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

/*
 * Return whether this slot is selected.
 */
bool Slot::is_selected( void ) const
{
    return is_selected_;
}

/*
 * Set whether this slot is selected.
 */
void Slot::set_selected( bool is_selected )
{
    is_selected_ = is_selected;
}
