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
bool Inventory::resolve_definitions( const ItemSchema* schema )
{
	// Resolve all items.
	JUTIL::Set<Item*>::Iterator i;
	for (i = items_.begin(); i.has_next(); i.next()) {
		Item* item = i.get_value();
		if (!schema->resolve( item )) {
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
 * Insert an item into the inventory.
 */
bool Inventory::insert_item( Item* item )
{
	// Check if item has spot in inventory.
	items_.insert( item );
	if (can_insert( item )) {
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
 * Move an item to an index in the inventory book. Assumes valid index and
 * that the item exists in the inventory/excluded already.
 */
void Inventory::move_item( Item* item, unsigned int index )
{
	// Remove item from its 
	if (is_excluded( item )) {
		excluded_book_->remove_item( item );
	}
	else {
		book_->remove_item( item );
	}

	// Move to new slot.
	item->set_position( index );
	book_->insert_item( item, index );
}

/*
 * Remove an item from the inventory.
 */
void Inventory::remove_item( Item* item )
{
	// Remove item from slot.
	if (book_->has_item( item )) {
		book_->remove_item( item );
	}
	else if (excluded_book_->has_item( item )) {
		excluded_book_->remove_item( item );
	}

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
bool Inventory::can_insert( const Item* item ) const
{
	if (item->has_valid_flags()) {
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
void Inventory::resolve_excluded( void )
{
	unsigned int i = 0;
	while (i < excluded_book_->get_slot_count()) {
		Slot* slot = excluded_book_->get_slot( i );
		if (slot->has_item()) {
			Item* item = slot->get_item();
			if (can_insert( item )) {
				// Keep index; a new item may have moved here.
				excluded_book_->remove_item( item );
				insert_item( item );
				continue;
			}
		}

		// Move to next slot.
		i++;
	}
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