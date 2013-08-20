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
 * Add an item to be handled by the inventory.
 */
bool Inventory::add_item( Item* item )
{

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
			if (!inventory_book_.has_item( index )) {
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
    unsigned int i;
    unsigned int end = items_.get_length();
    for (i = 0; i < end; ++i) {
        Item* item = items_.at( i );
        JUTIL::BaseAllocator::destroy( item );
    }
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
bool Inventory::on_item_added( Item* item )
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
    unsigned int i;
    unsigned int end = items_.get_length();
    Item* deleted = nullptr;
    for (i = 0; i < end; ++i) {
        Item* item = items_.at( i );
        if (item->get_unique_id() == unique_id) {
            // Item exists, now find it in the containers.
            deleted = item;
            items_.erase( i );
            break;
        }
    }
    if (deleted == nullptr) {
        return false;
    }

    // Clear from slots.
    inventory_book_.remove_item( deleted );
    excluded_book_.remove_item( deleted );
    // selected_book_.remove_item( item );
}

/*
 * Handle an item's flags being updated by backend.
 */
bool Inventory::on_item_updated( uint64 unique_id, uint32 flags )
{
    // Find item with matching ID.
    unsigned int i;
    unsigned int end = items_.get_length();
    Item* updated = nullptr;
    for (i = 0; i < end; ++i) {
        Item* item = items_.at( i );
        if (item->get_unique_id() == unique_id) {
            // Item exists, now find it in the containers.
            updated = item;
            updated->set_inventory_flags( flags );
            items_.erase( i );
            break;
        }
    }
    if (updated == nullptr) {
        return false;
    }

    // If item is excluded, see if we can replace into inventory.
    unsigned int excluded_index;
    if (excluded_book_.contains_item( updated, &excluded_index )) {
        if (updated->has_valid_inventory_flags()) {
            excluded_book_.remove_item( updated );

            // Move current item to excluded if there is one.
            unsigned int position = updated->get_position();
            if (!inventory_book_.is_slot_empty( position )) {
                Item* replaced = inventory_book_.get_item( position );
                excluded_book_.set_item( excluded_index, replaced );
            }
            inventory_book_.set_item( position, updated );
        }
    }
    else {
        // Inventory item, just move/replace.
}