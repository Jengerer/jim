#include "slot_container.h"

/*
 * Check whether all slots are empty.
 */
bool SlotContainer::is_empty( void ) const
{
	unsigned int end = get_slot_count();
	for (unsigned int i = 0; i < end; ++i) {
		if (get_slot( i )->has_item()) {
			return false;
		}
	}

	return true;
}

/*
 * Check whether slot has an item. Assumes valid index.
 */
bool SlotContainer::is_slot_empty( unsigned int index ) const
{
	return get_slot( index )->has_item() == false;
}

/*
 * Check whether index is valid.
 */
bool SlotContainer::is_valid_index( unsigned int index ) const
{
	return index < get_slot_count();
}

/*
 * Checks whether an item exists in the container.
 */
bool SlotContainer::has_item( Item* item ) const
{
	// Find slot by index.
	unsigned int index = item->get_index();
	return is_valid_index( index ) && get_slot( index )->get_item() == item;
}

/*
 * Inserts an item into the container. Assumes valid index.
 */
void SlotContainer::insert_item( Item* item, unsigned int index ) const
{
	item->set_index( index );
	get_slot( index )->set_item( item );
}

/*
 * Removes an item from the container. Assumes it exists.
 */
void SlotContainer::remove_item( Item* item ) const
{
	// Find slot by index.
	unsigned int index = item->get_index();
	get_slot( index )->remove_item();
}

/*
 * Empties all slots of items.
 */
void SlotContainer::empty_slots( void ) const
{
	unsigned int end = get_slot_count();
	for (unsigned int i = 0; i < end; ++i) {
		get_slot( i )->remove_item();
	}
}