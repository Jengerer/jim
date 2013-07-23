#include "inventory.hpp"

/*
 * Inventory constructor.
 */
Inventory::Inventory( SlotBook* book, DynamicSlotBook* excluded )
{
    book_ = book;
    excluded_ = excluded;
}

/*
 * Inventory destructor.
 */
Inventory::~Inventory( void )
{
	delete_items();
}

/*
 * Initialize the inventory to default sizes.
 */
bool Inventory::set_inventory_size( unsigned int slots )
{
    book_->set_size( slots );
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
	// Add to item set.
	if (!items_.push( item )) {
        return false;
    }

	// Attempt to place item.
    unsigned int position = item->get_position();
    if (!place_item( item, position )) {
        remove_item( item );
        return false;
    }

    return true;
}

/*
 * Check if an item can be inserted.
 */
bool Inventory::can_place( const Item* item ) const
{
	if (item->has_valid_inventory_flags()) {
		unsigned int index = item->get_position();
		if (book_->is_valid_index( index )) {
			// Allow insertion to existing slot.
			return book_->get_item( index ) == nullptr;
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
		book_->set_item( index, item );
	}
	else {
        // Add to excluded, and expand if necessary.
		if (!excluded_->push_item( item )) {
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
    if (!book_->remove_item( item )) {
        excluded_->remove_item( item );
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
	book_->empty_slots();
    excluded_->empty_slots();
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
    unsigned int end = excluded_->get_size();
	for (i = 0; i < end; ++i) {
		Item* item = excluded_->get_item( i );
		if ((item != nullptr) && can_place( item )) {
			unsigned int index = item->get_position();
            place_item( item, index );
		}
	}

	return true;
}