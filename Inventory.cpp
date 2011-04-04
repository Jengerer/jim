#include "Inventory.h"

Inventory::Inventory( 
	int width, int height, 
	int pages, int excludedWidth )
{
	width_ = width;
	height_ = height;
	pages_ = pages;
	excludedWidth_ = excludedWidth;

	setExcludedPage( 0 );
	createSlots();
}

Inventory::~Inventory()
{
	clearItems();
	removeSlots();
}

int Inventory::getWidth() const
{
	return width_;
}

int Inventory::getHeight() const
{
	return height_;
}

int Inventory::getPages() const
{
	return pages_;
}

Slot* Inventory::getInventorySlot( int index )
{
	return inventorySlots_[ index ];
}

Slot* Inventory::getExcludedSlot( int index )
{
	return excludedSlots_[ index ];
}

int Inventory::inventorySize() const
{
	return getWidth() * getHeight() * getPages();
}

int Inventory::excludedSize() const
{
	return excludedWidth_;
}

// TODO: Sort the lists, use faster search.
Item* Inventory::getItem( uint64 id )
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

itemVector* Inventory::getInventoryItems()
{
	return &inventoryItems_;
}

itemVector* Inventory::getExcludedItems()
{
	return &excludedItems_;
}

void Inventory::createSlots()
{
	// Generate slot arrays.
	inventorySlots_ = (Slot**)malloc( sizeof( Slot* ) * inventorySize() );
	excludedSlots_ = (Slot**)malloc( sizeof( Slot* ) * excludedSize() );

	// Create slots.
	int length = inventorySize();
	for (int i = 0; i < length; i++) {
		Slot *slot = new Slot( i );
		slot->setGroup( GROUP_INVENTORY );
		slot->setIndex( i );
		inventorySlots_[i] = slot;
	}

	// Create excluded slots.
	length = excludedSize();
	for (int i = 0; i < length; i++) {
		Slot *slot = new Slot();
		slot->setGroup( GROUP_EXCLUDED );
		excludedSlots_[i] = slot;
	}
}

void Inventory::removeSlots()
{
	int length;

	// Remove inventory.
	if (inventorySlots_ != nullptr) {
		length = inventorySize();
		for (int i = 0; i < length; i++) {
			Slot *slot = inventorySlots_[i];
			delete slot;
		}
	}

	free( inventorySlots_ );
	inventorySlots_ = nullptr;

	// Remove excluded.
	if (excludedSlots_ != nullptr) {
		length = excludedSize();
		for (int i = 0; i < length; i++) {
			Slot *slot = excludedSlots_[i];
			delete slot;
		}
	}

	free( excludedSlots_ );
	excludedSlots_ = nullptr;
}

void Inventory::emptySlots()
{
	// First remove items from slots.
	int i, length;
	if (inventorySlots_ != 0) {
		length = inventorySize();
		for (i = 0; i < length; i++) {
			Slot* slot = inventorySlots_[i];
			slot->setItem( 0 );
		}
	}

	if (excludedSlots_ != 0) {
		length = excludedSize();
		for (i = 0; i < length; i++) {
			Slot* slot = excludedSlots_[i];
			slot->setItem( 0 );
		}
	}
}

int Inventory::getExcludedPage() const
{
	return excludedPage_;
}

void Inventory::setExcludedPage( int page )
{
	excludedPage_ = page;
}

void Inventory::updateExcluded() {
	int start = getExcludedPage() * excludedSize();
	int itemCount = excludedItems_.size();
	for (int i = 0; i < excludedSize(); i++) {
		Slot *slot = excludedSlots_[i];

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
		Slot* destination = getInventorySlot( position );
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
			Slot *slot = getInventorySlot( position );
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

bool Inventory::canMove ( uint8 index )
{
	int maxSlot = inventorySize();
	if ((index >= 0) && (index < maxSlot)) {
		return getInventorySlot( index )->getItem() == 0;
	}
	
	return false;
}