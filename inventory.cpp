#include "inventory.hpp"

// Inventory attributes.
const unsigned int INVENTORY_PAGE_WIDTH = 10;
const unsigned int INVENTORY_PAGE_HEIGHT = 5;
const unsigned int EXCLUDED_PAGE_WIDTH = 5;
const unsigned int EXCLUDED_PAGE_HEIGHT = 1;
const unsigned int TRIAL_ACCOUNT_PAGES = 1;
const unsigned int PREMIUM_ACCOUNT_PAGES = 6;

/*
 * Inventory constructor.
 */
Inventory::Inventory( void )
    : inventory_book_( INVENTORY_PAGE_WIDTH, INVENTORY_PAGE_HEIGHT ),
      excluded_book_( EXCLUDED_PAGE_WIDTH, EXCLUDED_PAGE_HEIGHT ),
      schema_( nullptr )
{
}

/*
 * Inventory destructor.
 */
Inventory::~Inventory( void )
{
    // Destroy schema if loaded.
    JUTIL::BaseAllocator::safe_destroy( &schema_ );
	delete_items();
}

/*
 * Get the handle to the inventory book.
 */
SlotBook* Inventory::get_inventory_book( void )
{
    return &inventory_book_;
}

/*
 * Get the handle to the excluded book.
 */
DynamicSlotBook* Inventory::get_excluded_book( void )
{
    return &excluded_book_;
}

/*
 * Get a handle to the schema the inventory has, if any.
 */
ItemSchema* Inventory::get_schema( void )
{
    return schema_;
}

/*
 * Find item by unique ID.
 */
Item* Inventory::find_item( uint64 unique_id )
{
    Item* item = items_.find( unique_id );
    return item;
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
bool Inventory::place_item( Item* item )
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
 * Delete all items in the inventory.
 */
void Inventory::delete_items( void )
{
	items_.clear();
    inventory_book_.empty_slots();
    excluded_book_.empty_slots();
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
            place_item( item );
		}
	}

    // TODO: compress excluded here? Don't know if we have to do that anymore.
	return true;
}

/*
 * Handle item creation event.
 */
bool Inventory::on_item_created( Item* item )
{
    // Add to item set.
	if (!items_.add( item )) {
        return false;
    }

	// Attempt to place item if schema is loaded.
    if (schema_ != nullptr) {
        // Resolve definition.
        if (!schema_->resolve( item )) {
            return false;
        }

        // Place item into inventory.
        if (!place_item( item )) {
            return false;
        }
    }

    return true;
}

/*
 * Handle item deletion event.
 */
bool Inventory::on_item_deleted( uint64 unique_id )
{
    // Try to find the item with the matching ID.
    Item* deleted = items_.find( unique_id );
    if (deleted == nullptr) {
        return false;
    }
	items_.destroy( deleted );

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
	Item* updated = items_.find( unique_id );
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
    return true;
}

/*
 * Handle craft failure event.
 */
bool Inventory::on_craft_failed( void )
{
    return true;
}

/*
 * Handle inventory reset event.
 */
bool Inventory::on_inventory_reset( void )
{
    delete_items();
    return true;
}

/*
 * Handle inventory size change event.
 */
bool Inventory::on_inventory_resize( bool is_trial_account, uint32 extra_slots )
{
    // Calculate number of slots to resize to.
    unsigned int total_slots = INVENTORY_PAGE_WIDTH * INVENTORY_PAGE_HEIGHT;
    if (is_trial_account) {
        // Should be no extra slots for trial accounts.
        total_slots *= TRIAL_ACCOUNT_PAGES;
    }
    else {
        total_slots = (total_slots * PREMIUM_ACCOUNT_PAGES) + extra_slots;
    }
    if (total_slots < inventory_book_.get_size()) {
        return false;
    }

    // Attempt to set inventory book size.
    if (!inventory_book_.set_size( total_slots )) {
        return false;
    }
    return true;
}

/*
 * Handle inventory load completion event; notify to start loading schema.
 */
bool Inventory::on_inventory_loaded( void )
{
    return true;
}

/*
 * Handle schema load completed event.
 */
bool Inventory::on_schema_loaded( ItemSchema* schema )
{
    // Resolve and place all items.
    unsigned int i;
    unsigned int end = items_.get_size();
    for (i = 0; i < end; ++i) {
        Item* item = items_.get_item( i );

        // Find definition for item.
        if (!schema->resolve( item )) {
            return false;
        }

        // Try to place item.
        unsigned int position = item->get_position();
        if (!place_item( item )) {
            return false;
        }
    }

    schema_ = schema;
    return true;
}