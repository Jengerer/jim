#include "inventory.hpp"

// Inventory attributes.
const unsigned int INVENTORY_PAGE_WIDTH = 10;
const unsigned int INVENTORY_PAGE_HEIGHT = 5;
const unsigned int EXCLUDED_PAGE_WIDTH = 5;
const unsigned int EXCLUDED_PAGE_HEIGHT = 1;

/*
 * Inventory constructor.
 */
Inventory::Inventory( void )
    : inventory_book_( INVENTORY_PAGE_WIDTH, INVENTORY_PAGE_HEIGHT ),
      excluded_book_( EXCLUDED_PAGE_WIDTH, EXCLUDED_PAGE_HEIGHT )
{
}

/*
 * Inventory destructor.
 */
Inventory::~Inventory( void )
{
	delete_items();
}

/*
 * Find item by unique ID.
 */
Item* Inventory::find_item( uint64 unique_id ) const
{
    unsigned int i;
    unsigned int length = items_.get_length();
    for (i = 0; i < length; ++i) {
        Item* item = items_.at( i );
        if (item->get_unique_id() == unique_id) {
            return item;
        }
    }

	return nullptr;
}

/*
 * Check if an item can be inserted.
 */
bool Inventory::can_place( const Item* item ) const
{
	if (item->has_valid_inventory_flags()) {
		unsigned int index = item->get_position();
		if (inventory_book_.is_valid_index( index )) {
			// Allow insertion to existing slot.
			if (inventory_book_.is_slot_empty( index )) {
                return true;
            }
		}
	}

	return false;
}

/*
 * Insert an item into the inventory.
 * Returns true if able to insert into inventory and excluded
 * book, false otherwise.
 */
bool Inventory::place_item( Item* item, unsigned int index )
{
	// Check if item has spot in inventory.
	if (can_place( item )) {
        unsigned int index = item->get_position();
		inventory_book_.set_item( index, item );
	}
	else {
        // Add to excluded, and expand if necessary.
		if (!excluded_book_.push_item( item )) {
            return false;
        }
	}

    return true;
}

/*
 * Remove item from slot containers, but keep in set.
 */
void Inventory::displace_item( Item* item )
{
	// Remove item from slot.
    if (!inventory_book_.remove_item( item )) {
        excluded_book_.remove_item( item );
    }
}

/*
 * Remove an item from the inventory.
 */
void Inventory::remove_item( Item* item )
{
	displace_item( item );
	items_.remove( item );
}

/*
 * Remove all items from inventory.
 */
void Inventory::remove_items( void )
{
	inventory_book_.empty_slots();
    excluded_book_.empty_slots();
    items_.clear();
}

/*
 * Delete all items in the inventory.
 */
void Inventory::delete_items( void )
{
	items_.clear();
    remove_items();
}

/*
 * Attempt to place excluded items into valid inventory slots.
 */
bool Inventory::resolve_excluded( void )
{
	unsigned int i = 0;
    unsigned int end = excluded_book_.get_size();
	for (i = 0; i < end; ++i) {
		Item* item = excluded_book_.get_item( i );
		if ((item != nullptr) && can_place( item )) {
			unsigned int index = item->get_position();
            place_item( item, index );
		}
	}

	return true;
}

/*
 * Handle item creation event.
 */
bool Inventory::on_item_created( Item* item )
{
	// Attempt to place item.
    unsigned int position = item->get_position();
    if (!place_item( item, position )) {
        remove_item( item );
        return false;
    }

    // Add to item set.
	if (!items_.push( item )) {
        return false;
    }

    return true;
}

/*
 * Handle item deletion event.
 */
bool Inventory::on_item_deleted( uint64 unique_id )
{
    // Try to find the item with the matching ID.
    Item* deleted = items_.get_item( unique_id );
    if (deleted == nullptr) {
        return false;
    }
	items_.delete_item( deleted );

    // Clear from slots.
    if (!inventory_book_.remove_item( deleted )) {
		excluded_book_.remove_item( deleted );
	}
    // TODO: selected_book_.remove_item( item );
	return true;
}

/*
 * Handle an item's flags being updated by backend.
 * Assumes that an excluded item will never be told to replace
 * an inventory item and that an inventory item will never need
 * to move to excluded.
 */
bool Inventory::on_item_updated( uint64 unique_id, uint32 flags )
{
    // Find item with matching ID.
	Item* updated = items_.get_item( unique_id );
	assert( updated != nullptr );
    if (updated == nullptr) {
        return false;
    }
	updated->set_inventory_flags( flags );

	// If this is an inventory item, swap with replaced, if exists.
	unsigned int index;
	unsigned int destination = updated->get_position();
	if (inventory_book_.contains_item( updated, &index )) {
		Item* replaced = inventory_book_.get_item( destination );
		inventory_book_.set_item( index, replaced );
	}
	inventory_book_.set_item( destination, updated );
}

/*
 * Handle craft failure event.
 */
bool Inventory::on_craft_failed( void )
{
}