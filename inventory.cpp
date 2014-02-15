#include "inventory.hpp"

// Inventory attributes.
const unsigned int TRIAL_SLOT_COUNT = 50;
const unsigned int PREMIUM_SLOT_COUNT = 300;

/*
 * Inventory constructor.
 */
Inventory::Inventory( void )
{
}

/*
 * Inventory destructor.
 */
Inventory::~Inventory( void )
{
}

/*
 * Setting inventory listener handle.
 */
void Inventory::set_listener( InventoryListener* listener )
{
	listener_ = listener;
}

/*
 * Get the handle to the inventory book.
 */
SlotArray* Inventory::get_inventory_slots( void )
{
    return &inventory_slots_;
}

/*
 * Get the handle to the excluded book.
 */
DynamicSlotArray* Inventory::get_excluded_slots( void )
{
    return &excluded_slots_;
}

/*
 * Get the handle to the selected slot array.
 */
DynamicSlotArray* Inventory::get_selected_slots( void )
{
    return &selected_slots_;
}

/*
 * Get a handle to the schema the inventory has, if any.
 */
ItemSchema* Inventory::get_schema( void )
{
    return &schema_;
}

/*
 * Handle schema load completed event.
 */
bool Inventory::on_schema_loaded( JUI::Graphics2D *graphics )
{
    // Resolve and place all items.
    unsigned int i;
    unsigned int end = items_.get_size();
    for (i = 0; i < end; ++i) {
        Item* item = items_.get_item( i );

        // Find definition for item.
        if (!schema_.resolve( item, graphics )) {
            return false;
        }

        // Try to place item.
        unsigned int position = item->get_position();
        if (!place_item( item )) {
            return false;
        }
    }

    return true;
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
		if (inventory_slots_.is_valid_index( index )) {
			// Allow insertion to existing slot.
			if (inventory_slots_.is_slot_empty( index )) {
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
		inventory_slots_.set_item( index, item );
	}
	else {
        // Add to excluded, and expand if necessary.
		if (!excluded_slots_.push_item( item )) {
            return false;
        }
	}

    return true;
}

/*
 * Move an item to an inventory slot and notify the listener.
 * This function should only be used if we want listeners to hear
 * about movement; not when filling/initializing the inventory.
 * The item's previous slot, if empty, should be cleaned externally
 * and is not handled here.
 */
bool Inventory::move_item( Item* item, unsigned int index )
{
	// Position in inventory and update flags.
	item->set_position( index );
	inventory_slots_.set_item( index, item );

	// Notify listener.
	if (!listener_->on_item_moved( item )) {
		return false;
	}
	return true;
}

/*
 * Remove item from slot containers, but keep in set.
 */
void Inventory::displace_item( Item* item )
{
	// Remove item from slot.
    if (inventory_slots_.remove_item( item )) {
		return;
	}
	if (excluded_slots_.remove_item( item )) {
		excluded_slots_.compress_slots();
	}
}

/*
 * Delete all items in the inventory.
 */
void Inventory::delete_items( void )
{
	items_.clear();
    inventory_slots_.empty_slots();
    excluded_slots_.empty_slots();
}

/*
 * Toggle slot disables to allow valid movements.
 */
void Inventory::set_slot_mode( InventorySlotMode slot_mode )
{
	if (slot_mode == SLOT_MODE_ENABLE_ALL) {
		inventory_slots_.set_enabled( true );
		excluded_slots_.set_enabled( true );
	}
	else {
		excluded_slots_.set_enabled( false );
		if (slot_mode == SLOT_MODE_RESTRICTED_DRAG) {
			unsigned int i;
			unsigned int end = inventory_slots_.get_size();;
			unsigned int selected_count = selected_slots_.get_size();
			unsigned int valid_count = 0;
			// Only enable inventory slots that we can start a fit from.
			for (i = 0; i < end; ++i) {
				unsigned int back_index = end - (i + 1);
				const Slot* slot = inventory_slots_.get_slot( back_index );
				Item* item = slot->get_item();
				bool enabled = !slot->has_item() || item->is_selected();
				if (enabled) {
					++valid_count;
					enabled = (valid_count >= selected_count);
				}
				inventory_slots_.set_enabled( back_index, enabled );
			}
		}
		else {
			inventory_slots_.set_enabled( true );
		}
	}
}

/*
 * Toggle selection of a specific item.
 */
bool Inventory::set_selected( Item* item, bool is_selected )
{
    // Toggle selection state in container.
    if (is_selected) {
        if (!selected_slots_.push_item( item )) {
            return false;
        }
    }
    else {
        selected_slots_.remove_item( item );
		selected_slots_.compress_slots();
    }

	return true;
}

/*
 * Remove all items from selection.
 */
void Inventory::clear_selection( void )
{
	unsigned int i;
	for (i = 0; i < selected_slots_.get_size(); ++i) {
		Item* item = selected_slots_.get_item( i );
		item->set_selected( false );

		// Unset selected state from inventory/excluded.
		unsigned int j;
		if (inventory_slots_.contains_item( item, &j )) {
			inventory_slots_.update_slot( j );
		}
		if (excluded_slots_.contains_item( item, &j )) {
			excluded_slots_.update_slot( j );
		}
	}
	selected_slots_.destroy_slots();
}

/*
 * Attempt to place excluded items into valid inventory slots.
 */
bool Inventory::resolve_excluded( void )
{
	unsigned int i = 0;
    unsigned int end = excluded_slots_.get_size();
	for (i = 0; i < end; ++i) {
		Item* item = excluded_slots_.get_item( i );
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
    if (schema_.is_loaded()) {
        // Resolve definition.
        if (!schema_.resolve( item, nullptr )) {
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
    if (!inventory_slots_.remove_item( deleted )) {
		excluded_slots_.remove_item( deleted );
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

	// If this is an inventory item, swap/empty new slot with old.
	unsigned int old_index;
	unsigned int destination = updated->get_position();
	if (inventory_slots_.contains_item( updated, &old_index )) {
		Item* replaced = inventory_slots_.get_item( destination );
		inventory_slots_.set_item( old_index, replaced );
	}

	// Get new position and move into it.
	inventory_slots_.set_item( destination, updated );
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
    unsigned int total_slots;
    if (is_trial_account) {
        // Should be no extra slots for trial accounts.
        total_slots = TRIAL_SLOT_COUNT;
    }
    else {
        total_slots = PREMIUM_SLOT_COUNT + extra_slots;
    }
    if (total_slots < inventory_slots_.get_size()) {
        return false;
    }

    // Attempt to set inventory book size.
    if (!inventory_slots_.set_size( total_slots )) {
        return false;
    }
    return true;
}

/*
 * Handle inventory load completion event; notify to start loading schema.
 */
bool Inventory::on_inventory_loaded( void )
{
	if (!listener_->on_inventory_loaded()) {
		return false;
	}
    return true;
}