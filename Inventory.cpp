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
	createSlots();
}

Inventory::~Inventory()
{
	clearItems();
	removeSlots();
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

int Inventory::GetInventorySize() const
{
	return GetPageCapacity() * GetPageCount();
}

int Inventory::GetPageCapacity() const
{
	return GetWidth() * GetHeight();
}

int Inventory::GetExcludedSize() const
{
	return excludedWidth_;
}

// TODO: Sort the lists, use faster search.
Item* Inventory::GetItem( uint64 id )
{
	itemVector::iterator i;

	// Check all inventory.
	for each(Item *item in inventoryItems_) {
		if (item->getUniqueId() == id) {
			return item;
		}
	}

	// Check all excluded.
	for each(Item *item in excludedItems_) {
		if (item->getUniqueId() == id) {
			return item;
		}
	}

	return nullptr;
}

const itemVector*	Inventory::GetInventoryItems() const
{
	return &inventoryItems_;
}

const itemVector*	Inventory::GetExcludedItems() const
{
	return &excludedItems_;
}

void Inventory::createSlots()
{
	// Generate slot arrays.
	inventorySlots_ = new Slot[ GetInventorySize() ];
	excludedSlots_ = new Slot[ GetExcludedSize() ];

	// Create slots.
	int length = GetInventorySize();
	for (int i = 0; i < length; i++) {
		Slot *slot = &inventorySlots_[ i ];
		slot->setGroup( GROUP_INVENTORY );
		slot->setIndex( i );
	}

	// Create excluded slots.
	length = GetExcludedSize();
	for (int i = 0; i < length; i++) {
		Slot *slot = &excludedSlots_[ i ];
		slot->setGroup( GROUP_EXCLUDED );
	}
}

void Inventory::AddSlots( unsigned int numSlots )
{
	int extraPages = numSlots / (GetPageCapacity());
	SetPageCount( GetPageCount() + extraPages );
}

void Inventory::removeSlots()
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

void Inventory::emptySlots()
{
	int i, length;
	if (inventorySlots_ != 0) {
		length = GetInventorySize();
		for (i = 0; i < length; i++) {
			Slot* slot = &inventorySlots_[i];
			slot->setItem( 0 );
		}
	}

	if (excludedSlots_ != 0) {
		length = GetExcludedSize();
		for (i = 0; i < length; i++) {
			Slot* slot = &excludedSlots_[i];
			slot->setItem( 0 );
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

void Inventory::updateExcluded() {
	int start = GetExcludedPage() * GetExcludedSize();
	int itemCount = excludedItems_.size();
	for (int i = 0; i < GetExcludedSize(); i++) {
		Slot *slot = &excludedSlots_[i];

		int index = start + i;
		if (index >= itemCount) {
			slot->setItem( 0 );
		}
		else {
			Item *item = excludedItems_[index];
			slot->setItem( item );
		}
	}
}

// Inserts an item, adds it to list, and returns its slot, if any.
Slot* Inventory::addItem( Item* item )
{
	Slot* slot = insertItem( item );
	if (slot == nullptr || slot->getGroup() == GROUP_EXCLUDED) {
		excludedItems_.push_back( item );
		updateExcluded();
	}
	else {
		inventoryItems_.push_back( item );
	}

	return slot;
}

// Inserts an item and returns its slot, if any.
Slot* Inventory::insertItem( Item* item )
{
	// Add item to correct slot.
	uint32 flags = item->getFlags();
	uint16 position = item->getIndex() - 1;
	uint32 leftBytes = flags & 0xf0000000;
	if ((leftBytes == 0x80000000) && (flags & 0xfff) && canMove( position )) {
		Slot* destination = GetInventorySlot( position );
		destination->setItem( item );
		return destination;
	}

	return nullptr;
}

void Inventory::removeItem( Item* item )
{
	// Search for item in inventory.
	itemVector::iterator i;
	for (i = inventoryItems_.begin(); i != inventoryItems_.end(); i++) {
		Item *current = *i;
		if (item == current) {
			int position = item->getIndex() - 1;
			Slot *slot = GetInventorySlot( position );
			slot->setItem( 0 );
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
			updateExcluded();
			
			delete item;
			return;
		}
	}
}

void Inventory::clearItems()
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

void Inventory::moveItem( Slot *source, Slot *destination )
{
	// Check if needs transfer.
	Item* target = source->getItem();
	if (source->getGroup() == GROUP_EXCLUDED) {
		// Disallow movement to occupied inventory.
		Item* other = destination->getItem();
		if (destination->getGroup() == GROUP_INVENTORY && other == 0) {
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
			updateExcluded();
		}
	}
	else {
		source->setItem( destination->getItem() );
	}

	// Move items.
	destination->setItem( target );
}

bool Inventory::canMove ( uint16 index )
{
	int maxSlot = GetInventorySize();
	if ((index >= 0) && (index < maxSlot)) {
		return GetInventorySlot( index )->getItem() == 0;
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