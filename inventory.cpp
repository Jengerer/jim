#include "inventory.h"

#include <algorithm>

Inventory::Inventory( unsigned int inventorySize, unsigned int excludedSize )
{
	inventorySlots_ = new SlotVector( inventorySize );
	excludedSlots_ = new SlotVector( excludedSize );
	excludedPage_ = 0;
}

Inventory::~Inventory()
{
	remove_items();
	remove_slots();
}

const SlotVector* Inventory::get_inventory_slots() const
{
	return inventorySlots_;
}

const SlotVector* Inventory::get_excluded_slots() const
{
	return excludedSlots_;
}

void Inventory::remove_slots()
{
	if (inventorySlots_ != nullptr) {
		delete inventorySlots_;
		inventorySlots_ = nullptr;
	}

	if (excludedSlots_ != nullptr) {
		delete excludedSlots_;
		excludedSlots_ = nullptr;
	}
}

void Inventory::add_slots( unsigned int slots )
{
	inventorySlots_->add_slots( slots );
}

void Inventory::empty_slots()
{
	inventorySlots_->empty_slots();
	excludedSlots_->empty_slots();
}

unsigned int Inventory::GetInventorySize() const
{
	return inventorySlots_->get_slot_count();
}

unsigned int Inventory::GetExcludedSize() const
{
	return excludedSlots_->get_slot_count();
}

Item* Inventory::GetItemByUniqueId( uint64 uniqueId )
{
	for (auto i = inventoryItems_.begin(), end = inventoryItems_.end(); i != end; ++i) {
		Item* item = *i;
		if (item->get_unique_id() == uniqueId) {
			return item;
		}
	}

	for (auto i = excludedItems_.begin(), end = excludedItems_.end(); i != end; ++i) {
		Item* item = *i;
		if (item->get_unique_id() == uniqueId) {
			return item;
		}
	}

	return nullptr;
}

//=============================================================
// Purpose: Inserts an item into the inventory or excluded.
// Notes:	A call to update_excluded should be called after
//			any series of inserts.
// Return:	The inventory slot inserted into if succeeded,
//			nullptr if it was added to excluded.
//=============================================================
Slot* Inventory::insert_item( Item* item )
{
	// Check if we should/can place in inventory.
	if (CanInsert( item )) {
		Slot* destination = inventorySlots_->get_slot_by_index( item->get_position() );
		destination->set_item( item );
		ToInventory( item );
		return destination;
	}

	ToExcluded( item );
	return nullptr;
}

void Inventory::remove_item( Item* item )
{
	// Find the inventory slot.
	auto i = inventoryItems_.find( item );
	if (i != inventoryItems_.end()) {
		inventorySlots_->remove_item( item );
		inventoryItems_.erase( i );
	}
	else {
		excludedItems_.erase( item );
		update_excluded();
	}
}

void Inventory::remove_items()
{
	// Delete inventory items.
	for (auto i = inventoryItems_.begin(); i != inventoryItems_.end(); i = inventoryItems_.erase( i )) {
		delete *i;
	}

	// Delete excluded items.
	for (auto i = excludedItems_.begin(); i != excludedItems_.end(); i = excludedItems_.erase( i )) {
		delete *i;
	}
}

bool Inventory::CanInsert( const Item* item ) const
{
	if (item->has_valid_flags()) {
		uint16 index = item->get_position();
		return inventorySlots_->is_valid_index( index ) && inventorySlots_->is_slot_empty( index );
	}

	return false;
}

void Inventory::SetExcludedPage( unsigned int excludedPage )
{
	excludedPage_ = excludedPage;
}

void Inventory::update_excluded( void )
{
	excludedSlots_->empty_slots();

	// Push iterator up to current page.
	unsigned int iteratorIndex = 0;
	unsigned int startIndex = excludedPage_ * excludedSlots_->get_slot_count();
	auto i = excludedItems_.begin();
	while (i != excludedItems_.end() && iteratorIndex < startIndex) {
		i++;
		iteratorIndex++;
	}

	// Now set the rest of the slots.
	unsigned endIndex = startIndex + excludedSlots_->get_slot_count();
	for (unsigned int slotIndex = startIndex; slotIndex < endIndex && i != excludedItems_.end(); slotIndex++) {
		Slot* slot = excludedSlots_->get_slot_by_index( slotIndex );
		slot->set_item( *i++ );
	}
}

void Inventory::resolve_excluded( void )
{
	auto i = excludedItems_.begin();
	while (i != excludedItems_.end()) {
		Item* item = *i;
		if (CanInsert( item )) {
			i = excludedItems_.erase( i );
			insert_item( item );
		}
		else {
			i++;
		}
	}
}

void Inventory::ToExcluded( Item* item )
{
	inventoryItems_.erase( item );
	excludedItems_.insert( item );
}

void Inventory::ToInventory( Item* item )
{
	excludedItems_.erase( item );
	inventoryItems_.insert( item );
}