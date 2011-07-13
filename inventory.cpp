#include "inventory.h"

#include <algorithm>

Inventory::Inventory( unsigned int inventorySize, unsigned int excludedSize )
{
	excludedPage_ = 0;
	inventorySize_ = inventorySize;
	excludedSize_ = excludedSize;
	CreateSlots();
}

Inventory::~Inventory()
{
	RemoveItems();
	RemoveSlots();
}

void Inventory::CreateSlots()
{
	// Generate slot arrays.
	inventory_ = new SlotVector( GetInventorySize() );
	excluded_ = new SlotVector( GetExcludedSize() );
}

void Inventory::RemoveSlots()
{
	if (inventory_ != nullptr) {
		delete inventory_;
		inventory_ = nullptr;
	}

	if (excluded_ != nullptr) {
		delete excluded_;
		excluded_ = nullptr;
	}
}

void Inventory::AddSlots( unsigned int slots )
{
	inventory_->AddSlots( slots );
}

void Inventory::EmptySlots()
{
	inventory_->EmptySlots();
	excluded_->EmptySlots();
}

unsigned int Inventory::GetInventorySize() const
{
	return inventorySize_;
}

unsigned int Inventory::GetExcludedSize() const
{
	return excludedSize_;
}

Item* Inventory::GetItemByUniqueId( uint64 uniqueId, bool shouldRemove )
{
	vector<Item*>::iterator i, end;
	for (i = inventoryItems_.begin(), end = inventoryItems_.end(); i != end; ++i) {
		Item* item = *i;
		if (item->GetUniqueId() == uniqueId) {
			if (shouldRemove) {
				inventoryItems_.erase( i );
			}

			return item;
		}
	}

	for (i = excludedItems_.begin(), end = excludedItems_.end(); i != end; ++i) {
		Item* item = *i;
		if (item->GetUniqueId() == uniqueId) {
			if (shouldRemove) {
				excludedItems_.erase( i );
			}

			return item;
		}
	}

	return nullptr;
}

//=============================================================
// Purpose: Inserts an item into the inventory or excluded.
// Notes:	A call to UpdateExcluded should be called after
//			any series of inserts.
//=============================================================
void Inventory::InsertItem( Item* item )
{
	// Check if we should/can place in inventory.
	uint16 index = item->GetIndex();
	if (item->HasValidFlags() && inventory_->IsSlotEmpty( index )) {
		Slot* destination = inventory_->GetSlotByIndex( index );
		destination->SetItem( item );
		ToInventory( item );
	}
	else {
		ToExcluded( item );
	}
}

void Inventory::RemoveItem( uint64 uniqueId )
{
	// Remove it from slots.
	Item* targetItem = GetItemByUniqueId( uniqueId, true );
	if (targetItem != nullptr) {
		Slot* targetSlot = inventory_->GetSlotByItem( targetItem );
		if (targetSlot == nullptr) {
			targetSlot = excluded_->GetSlotByItem( targetItem );
			if (targetSlot != nullptr) {
				excluded_->RemoveItem( targetItem );
			}
		}
		else {
			inventory_->RemoveItem( targetItem );
		}
	}
}

void Inventory::RemoveItems()
{
	vector<Item*>::iterator i;
	for (i = inventoryItems_.begin(); i != inventoryItems_.end(); i = inventoryItems_.erase( i )) {
		delete *i;
	}

	for (i = excludedItems_.begin(); i != excludedItems_.end(); i = excludedItems_.erase( i )) {
		delete *i;
	}
}

void Inventory::SetExcludedPage( unsigned int excludedPage )
{
	excludedPage_ = excludedPage;
}

void Inventory::UpdateExcluded( void )
{
	unsigned int itemIndex = excludedPage_ * excludedSize_;
	for (unsigned int slotIndex = 0; excluded_->IsValidIndex( slotIndex ); ++slotIndex) {
		Slot* slot = excluded_->GetSlotByIndex( slotIndex );
		if (itemIndex < excludedItems_.size()) {
			slot->SetItem( itemIndex < excludedItems_.size() ? excludedItems_[itemIndex++] : nullptr );
		}
	}
}

//=============================================================
// Purpose: Attempt to place items with valid positions
//			back into the inventory.
//=============================================================
void Inventory::ResolveExcluded( void )
{
	vector<Item*>::iterator i, end;
	for (i = excludedItems_.begin(), end = excludedItems_.end(); i != end; ++i) {
		Item* item = *i;
		InsertItem( item );
	}

	UpdateExcluded();
}

// NOTE: Should call UpdateExcluded after using this.
void Inventory::ToInventory( Item *item )
{
	// Remove from excluded.
	vector<Item*>::iterator i = find( excludedItems_.begin(), excludedItems_.end(), item );
	if (i != excludedItems_.end()) {
		excludedItems_.erase( i );
	}

	inventoryItems_.push_back( item );
}

// NOTE: Should call UpdateExcluded after using this.
void Inventory::ToExcluded( Item *item )
{
	// Remove from inventory.
	vector<Item*>::iterator i = find( inventoryItems_.begin(), inventoryItems_.end(), item );
	if (i != inventoryItems_.end()) {
		inventory_->RemoveItem( item );
		inventoryItems_.erase( i );
	}

	excludedItems_.push_back( item );
}