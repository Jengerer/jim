#include "inventory.hpp"

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
 * Get number of items in item set.
 */
unsigned int Inventory::get_item_count( void ) const
{
	return items_.get_size();
}

/*
 * Get indexed item from item set.
 */
Item* Inventory::get_item( unsigned int index )
{
	return items_.get_item( index );
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
 * Find item by unique ID.
 */
Item* Inventory::find_item( uint64 unique_id )
{
    Item* item = items_.find( unique_id );
    return item;
}

/*
 * Add an item to the item set.
 */
bool Inventory::add_item( Item* item )
{
	if (!items_.add( item )) {
		return false;
	}
	return true;
}

/*
 * Delete item from the inventory.
 */
void Inventory::delete_item( Item* item )
{
	// Try to find the item with the matching ID.
	items_.destroy( item );

    // Clear from slots.
    if (!inventory_slots_.remove_item( item )) {
		excluded_slots_.remove_item( item );
		excluded_slots_.compress_slots();
	}
    selected_slots_.remove_item( item );
	selected_slots_.compress_slots();
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
	selected_slots_.empty_slots();
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

	listener_->on_selection_changed();
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
	listener_->on_selection_changed();
}
