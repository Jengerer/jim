#include "Inventory.h"

Inventory::Inventory( 
	int width, int height, 
	int pages, int excludedWidth )
{
	width_ = width;
	height_ = height;
	pages_ = pages;
	excludedWidth_ = excludedWidth;

	SetExcludedPage( 0 );
	CreateSlots();
}

Inventory::~Inventory()
{
	ClearItems();
	RemoveSlots();
}

int Inventory::GetWidth() const
{
	return width_;
}

int Inventory::GetHeight() const
{
	return height_;
}

int Inventory::GetPageCount() const
{
	return pages_;
}

Slot* Inventory::GetInventorySlot( int index )
{
	return &inventorySlots_[ index ];
}

Slot* Inventory::GetExcludedSlot( int index )
{
	return &excludedSlots_[ index ];
}

int Inventory::GetInventorySize( void ) const
{
	return GetPageSize() * GetPageCount();
}

int Inventory::GetPageSize( void ) const
{
	return GetWidth() * GetHeight();
}

int Inventory::GetExcludedSize() const
{
	return excludedWidth_;
}

// TODO: Sort the lists, use faster search.
Item* Inventory::GetItemByUniqueId( uint64 id )
{
	itemVector::iterator i;

	// Check all inventory.
	for each(Item *item in inventoryItems_) {
		if (item->GetUniqueId() == id) {
			return item;
		}
	}

	// Check all excluded.
	for each(Item *item in excludedItems_) {
		if (item->GetUniqueId() == id) {
			return item;
		}
	}

	return nullptr;
}

const itemVector*	Inventory::GetInventoryItems( void ) const
{
	return &inventoryItems_;
}

const itemVector*	Inventory::GetExcludedItems( void ) const
{
	return &excludedItems_;
}

void Inventory::CreateSlots( void )
{
	// Generate slot arrays.
	inventorySlots_ = new Slot[ GetInventorySize() ];
	excludedSlots_ = new Slot[ GetExcludedSize() ];

	// Create slots.
	int length = GetInventorySize();
	for (int i = 0; i < length; i++) {
		Slot *slot = &inventorySlots_[ i ];
		slot->SetGroup( GROUP_INVENTORY );
		slot->SetIndex( i );
	}

	// Create excluded slots.
	length = GetExcludedSize();
	for (int i = 0; i < length; i++) {
		Slot *slot = &excludedSlots_[ i ];
		slot->SetGroup( GROUP_EXCLUDED );
	}
}

void Inventory::AddSlots( unsigned int numSlots )
{
	int extraPages = numSlots / GetPageSize();
	SetPageCount( GetPageCount() + extraPages );
}

void Inventory::RemoveSlots( void )
{
	// Remove inventory.
	if (inventorySlots_ != nullptr) {
		delete[] inventorySlots_;
		inventorySlots_ = nullptr;
	}

	// Remove excluded.
	if (excludedSlots_ != nullptr) {
		delete[] excludedSlots_;
		excludedSlots_ = nullptr;
	}
}

void Inventory::EmptySlots( void )
{
	int i, length;
	if (inventorySlots_ != nullptr) {
		length = GetInventorySize();
		for (i = 0; i < length; i++) {
			Slot* slot = &inventorySlots_[i];
			slot->SetItem( nullptr );
		}
	}

	if (excludedSlots_ != nullptr) {
		length = GetExcludedSize();
		for (i = 0; i < length; i++) {
			Slot* slot = &excludedSlots_[i];
			slot->SetItem( nullptr );
		}
	}
}

int Inventory::GetExcludedPage() const
{
	return excludedPage_;
}

void Inventory::SetExcludedPage( int page )
{
	excludedPage_ = page;
}

void Inventory::UpdateExcluded( void ) {
	int start = GetExcludedPage() * GetExcludedSize();
	int itemCount = excludedItems_.size();
	for (int i = 0; i < GetExcludedSize(); i++) {
		Slot *slot = &excludedSlots_[i];

		int index = start + i;
		if (index >= itemCount) {
			slot->SetItem( nullptr );
		}
		else {
			Item *item = excludedItems_[index];
			slot->SetItem( item );
		}
	}
}

// Inserts an item, adds it to list, and returns its slot, if any.
Slot* Inventory::AddItem( Item* item )
{
	Slot* slot = InsertItem( item );
	if (slot == nullptr || slot->GetGroup() == GROUP_EXCLUDED) {
		excludedItems_.push_back( item );
		UpdateExcluded();
	}
	else {
		inventoryItems_.push_back( item );
	}

	return slot;
}

// Inserts an item and returns its slot, if any.
Slot* Inventory::InsertItem( Item* item )
{
	// Add item to correct slot.
	uint32 flags = item->GetFlags();
	uint16 position = item->GetIndex();
	uint32 leftBytes = flags & 0xf0000000;
	if ((leftBytes == 0x80000000) && (flags & 0xfff) && CanMove( position )) {
		Slot* destination = GetInventorySlot( position );
		destination->SetItem( item );
		return destination;
	}

	return nullptr;
}

void Inventory::RemoveItem( Item* item )
{
	// Search for item in inventory.
	itemVector::iterator i;
	for (i = inventoryItems_.begin(); i != inventoryItems_.end(); i++) {
		Item *current = *i;
		if (item == current) {
			int position = item->GetIndex();
			Slot *slot = GetInventorySlot( position );
			slot->SetItem( nullptr );
			inventoryItems_.erase( i );

			delete item;
			return;
		}
	}

	// Search for item in excluded.
	for (i = excludedItems_.begin(); i != excludedItems_.end(); i++) {
		Item *current = *i;
		if (item == current) {
			excludedItems_.erase( i );
			UpdateExcluded();
			
			delete item;
			return;
		}
	}
}

void Inventory::ClearItems( void )
{
	for each(Item *item in inventoryItems_) {
		delete item;
	}

	for each(Item *item in excludedItems_) {
		delete item;
	}

	inventoryItems_.clear();
	excludedItems_.clear();
}

void Inventory::MoveItem( Slot *source, Slot *destination )
{
	// Check if needs transfer.
	Item* target = source->GetItem();
	if (source->GetGroup() == GROUP_EXCLUDED) {
		// Disallow movement to occupied inventory.
		Item* other = destination->GetItem();
		if (destination->GetGroup() == GROUP_INVENTORY && other == nullptr) {
			// Remove item from excluded.
			itemVector::iterator i;
			for (i = excludedItems_.begin(); i != excludedItems_.end(); i++) {
				Item* item = *i;
				if (item == target) {
					excludedItems_.erase( i );
					break;
				}
			}

			// Add to inventory.
			inventoryItems_.push_back( target );
			UpdateExcluded();
		}
	}
	else {
		source->SetItem( destination->GetItem() );
	}

	// Move items.
	destination->SetItem( target );
}

bool Inventory::CanMove ( uint16 index )
{
	int maxSlot = GetInventorySize();
	if ((index >= 0) && (index < maxSlot)) {
		return GetInventorySlot( index )->GetItem() == nullptr;
	}
	
	return false;
}

void Inventory::SetWidth( int width )
{
	width_ = width;
}

void Inventory::SetHeight( int height )
{
	height_ = height;
}

void Inventory::SetPageCount( int pageCount )
{
	pages_ = pageCount;
}