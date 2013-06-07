#include "inventory.hpp"

#include <algorithm>

/*
 * Inventory constructor.
 */
Inventory::Inventory( SlotBook* book, DynamicSlotBook* excluded )
{
	set_book( book );
	set_excluded_book( excluded );
}

/*
 * Inventory destructor.
 */
Inventory::~Inventory( void )
{
	remove_items();
}

/*
 * Resolve item definitions after they've been loaded.
 */
bool Inventory::resolve_definitions( const ItemSchema* schema, const DefinitionLoader* definition_loader )
{
	// Resolve all items.
	JUTIL::Set<Item*>::Iterator i;
	for (i = items_.begin(); i.has_next(); i.next()) {
		Item* item = i.get_value();
		if (!schema->resolve( item )) {
            return false;
        }

		// Add alt textures
		if(!definition_loader->get_alt_texture( item )){
			return false;
		}
	}
    return true;
}

/*
 * Get inventory slot book.
 */
const SlotBook* Inventory::get_book( void ) const
{
	return book_;
}

/*
 * Get excluded slot book.
 */
const DynamicSlotBook* Inventory::get_excluded_book( void ) const
{
	return excluded_book_;
}

/*
 * Find item by unique ID.
 */
Item* Inventory::find_item( uint64 unique_id ) const
{
	JUTIL::Set<Item*>::Iterator i;
	for (i = items_.begin(); i.has_next(); i.next()) {
		Item* item = i.get_value();
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
	if (!items_.insert( item )) {
		return false;
	}

	// Place item.
	return place_item( item );
}

/*
 * Insert an item into the inventory.
 */
bool Inventory::place_item( Item* item )
{
	// Check if item has spot in inventory.
	if (can_place( item )) {
		book_->insert_item( item, item->get_position() );
	}
	else {
        // Add to excluded, and expand if necessary.
		if (!excluded_book_->insert_item( item )) {
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
	if (book_->has_item( item )) {
		book_->remove_item( item );
	}
	else if (excluded_book_->has_item( item )) {
		excluded_book_->remove_item( item );
	}
}

/*
 * Move an item to an index in the inventory book. Assumes slot at
 * index is empty.
 */
void Inventory::move_item( Item* item, unsigned int index )
{
	// Move to new slot.
	displace_item( item );
	item->set_position( index );
	place_item( item );
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
	excluded_book_->empty_slots();

	// Delete all items.
	JUTIL::Set<Item*>::Iterator i;
	for (i = items_.begin(); i.has_next(); i.next()) {
		Item* item = i.get_value();
		JUTIL::BaseAllocator::destroy( item );
	}
	items_.clear();
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
			return book_->is_slot_empty( index );
		}
	}

	return false;
}

/*
 * Checks if an item is excluded. Assumes item is in inventory/excluded.
 */
bool Inventory::is_excluded( const Item* item ) const
{
	return excluded_book_->has_item( item );
}

/*
 * Attempt to place excluded items.
 */
bool Inventory::resolve_excluded( void )
{
	unsigned int i = 0;
	while (i < excluded_book_->get_slot_count()) {
		Slot* slot = excluded_book_->get_slot( i );
		if (slot->has_item()) {
			Item* item = slot->get_item();
			if (can_place( item )) {
				// Keep index; a new item may have moved here.
				excluded_book_->remove_item( item );
				if (!place_item( item )) {
					return false;
				}
				continue;
			}
		}

		// Move to next slot.
		i++;
	}

	return true;
}

/*
 * Set inventory book to manage.
 */
void Inventory::set_book( SlotBook* book )
{
	book_ = book;
}

/*
 * Set book for excluded items.
 */
void Inventory::set_excluded_book( DynamicSlotBook* excluded_book )
{
	excluded_book_ = excluded_book;
}