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
	inventorySlots_ = new SlotVector( GetInventorySize() );
	excludedSlots_ = new SlotVector( GetExcludedSize() );
}

void Inventory::RemoveSlots()
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

void Inventory::AddSlots( unsigned int slots )
{
	inventorySlots_->AddSlots( slots );
}

void Inventory::EmptySlots()
{
	inventorySlots_->EmptySlots();
	excludedSlots_->EmptySlots();
}

unsigned int Inventory::GetInventorySize() const
{
	return inventorySize_;
}

unsigned int Inventory::GetExcludedSize() const
{
	return excludedSize_;
}

Item* Inventory::GetItemByUniqueId( uint64 uniqueId )
{
	for (auto i = inventoryItems_.begin(), end = inventoryItems_.end(); i != end; ++i) {
		Item* item = *i;
		if (item->GetUniqueId() == uniqueId) {
			return item;
		}
	}

	for (auto i = excludedItems_.begin(), end = excludedItems_.end(); i != end; ++i) {
		Item* item = *i;
		if (item->GetUniqueId() == uniqueId) {
			return item;
		}
	}

	return nullptr;
}

//=============================================================
// Purpose: Inserts an item into the inventory or excluded.
// Notes:	A call to UpdateExcluded should be called after
//			any series of inserts.
// Return:	The inventory slot inserted into if succeeded,
//			nullptr if it was added to excluded.
//=============================================================
Slot* Inventory::InsertItem( Item* item )
{
	// Check if we should/can place in inventory.
	if (CanInsert( item )) {
		Slot* destination = inventorySlots_->GetSlotByIndex( item->GetPosition() );
		destination->SetItem( item );
		ToInventory( item );
		return destination;
	}

	ToExcluded( item );
	return nullptr;
}

void Inventory::RemoveItem( Item* item )
{
	// Find the inventory slot.
	auto i = inventoryItems_.find( item );
	if (i != inventoryItems_.end()) {
		inventorySlots_->RemoveItem( item );
		inventoryItems_.erase( i );
	}
	else {
		excludedItems_.erase( item );
		UpdateExcluded();
	}
}

void Inventory::RemoveItems()
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
	if (item->HasValidFlags()) {
		uint16 index = item->GetPosition();
		return inventorySlots_->IsValidIndex( index ) && inventorySlots_->IsSlotEmpty( index );
	}

	return false;
}

void Inventory::SetExcludedPage( unsigned int excludedPage )
{
	excludedPage_ = excludedPage;
}

void Inventory::UpdateExcluded( void )
{
	excludedSlots_->EmptySlots();

	// Push iterator up to current page.
	unsigned int iteratorIndex = 0;
	unsigned int startIndex = excludedPage_ * excludedSize_;
	auto i = excludedItems_.begin();
	while (i != excludedItems_.end() && iteratorIndex < startIndex) {
		i++;
		iteratorIndex++;
	}

	// Now set the rest of the slots.
	unsigned endIndex = startIndex + excludedSize_;
	for (unsigned int slotIndex = startIndex; slotIndex < endIndex && i != excludedItems_.end(); slotIndex++) {
		Slot* slot = excludedSlots_->GetSlotByIndex( slotIndex );
		slot->SetItem( *i++ );
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