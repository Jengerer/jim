#include "Inventory.h"

Inventory::Inventory( 
	int width, int height, 
	int pages )
{
	// Set dimensions.
	pageWidth_ = width;
	pageHeight_ = height;
	pages_ = pages;

	// Null array until generation.
	inventory_ = 0;
	excludedSlots_ = 0;
	generateSlots();
}

Inventory::~Inventory()
{
	// Inventory has been deleted.
	clearItems();
	clearSlots();
}

void Inventory::emptySlots()
{
	// First remove items from slots.
	int i, length;
	if (inventory_ != 0) {
		length = getCapacity();
		for (i = 0; i < length; i++) {
			Slot* slot = inventory_[i];
			slot->setItem( 0 );
		}
	}

	if (excludedSlots_ != 0) {
		length = EXCLUDED_WIDTH;
		for (i = 0; i < length; i++) {
			Slot* slot = excludedSlots_[i];
			slot->setItem( 0 );
		}
	}
}

void Inventory::clearItems()
{
	// Now delete all items.
	while (!items_.empty()) {
		// Get item.
		Item* item = items_.back();

		// Delete it.
		delete item;
		items_.pop_back();
	}

	// Delete excluded.
	while (!excludedItems_.empty()) {
		// Get item.
		Item* item = excludedItems_.back();

		// Delete it.
		delete item;
		excludedItems_.pop_back();
	}
}

void Inventory::clearSlots() {
	// Delete inventory.
	if ( inventory_ ) {
		free( inventory_ );
		inventory_ = 0;
	}

	// Delete excluded.
	if ( excludedSlots_ ) {
		free( excludedSlots_ );
		excludedSlots_ = 0;
	}
}

void Inventory::generateSlots()
{
	// Generate slot arrays.
	inventory_ = (Slot**)malloc( sizeof( Slot* ) * getCapacity() );
	excludedSlots_ = (Slot**)malloc( sizeof( Slot* ) * EXCLUDED_WIDTH );

	// Create slots.
	for (int i = 0; i < getCapacity(); i++) {
		Slot *slot = new Slot( i );
		slot->setGroup( GROUP_INVENTORY );
		slot->setIndex( i );
		inventory_[i] = slot;
	}

	// Create excluded slots.
	for (int i = 0; i < EXCLUDED_WIDTH; i++) {
		Slot *slot = new Slot();
		slot->setGroup( GROUP_EXCLUDED );
		excludedSlots_[i] = slot;
	}
}

Slot* Inventory::addItem( Item* item )
{
	// Add to list.
	Slot* slot = insert( item );
	if (slot == 0 || slot->getGroup() == GROUP_EXCLUDED) {
		excludedItems_.push_back( item );
	}
	else {
		items_.push_back( item );
	}

	return slot;
}

itemVector* Inventory::getItems()
{
	return &items_;
}

Slot* Inventory::insert( Item* item ) {
	// Add item to correct slot.
	uint32 flags = item->getFlags();
	uint16 position = item->getIndex() - 1;
	uint32 validFlags = flags & 0xf0000000;
	if ((validFlags == 0x80000000) && (flags & 0xfff) && isValidSlot( position )) {
		Slot* destination = inventory_[position];

		// Should not allow overlap.
		if (destination->getItem() != 0) {
			// Check if slot free in excluded.
			int i, length = EXCLUDED_WIDTH;
			for (i = 0; i < length; i++) {
				Slot *slot = excludedSlots_[i];
				if (slot->getItem() == 0) {
					slot->setItem( item );
					return slot;
				}
			}

			return 0;
		}
		else {
			destination->setItem( item );
			return destination;
		}
	}
	else {
		// Check if slot free in excluded.
		int i, length = EXCLUDED_WIDTH;
		for (i = 0; i < length; i++) {
			Slot *slot = excludedSlots_[i];
			if (slot->getItem() == 0) {
				slot->setItem( item );
				return slot;
			}
		}

		return 0;
	}
}

void Inventory::removeItem( uint64 uniqueId )
{
	Item *target = 0;

	// Check inventory.
	itemVector::iterator i;
	for (i = items_.begin(); i != items_.end(); i++) {
		Item *item = *i;
		if (item->getUniqueId() == uniqueId) {
			target = item;
			items_.erase( i );
			break;
		}
	}

	// If hasn't been found yet, check excluded.
	if (target == 0) {
		for (i = excludedItems_.begin(); i != excludedItems_.end(); i++) {
			Item *item = *i;
			if (item->getUniqueId() == uniqueId) {
				target = item;
				excludedItems_.erase( i );
				break;
			}
		}
	}

	// Now remove from slots.
	if (target != 0) {
		int j, length = getCapacity();
		for (j = 0; j < length; j++) {
			Slot *slot = inventory_[j];
			if (target == slot->getItem()) {
				slot->setItem( 0 );
			}
		}

		// Remove from excluded.
		for (j = 0; j < EXCLUDED_WIDTH; j++) {
			Slot *slot = excludedSlots_[j];
			if (target == slot->getItem()) {
				slot->setItem( 0 );
			}
		}

		// Remove item.
		delete target;
	}
}

void Inventory::move( Slot *source, Slot *destination )
{
	// Check if needs transfer.
	Item* target = source->getItem();
	if (source->getGroup() == GROUP_EXCLUDED) {
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
		items_.push_back( target );
		updateExcluded();
	}
	else {
		source->setItem( destination->getItem() );
	}

	// Move items.
	destination->setItem( target );
}

void Inventory::updateExcluded() {
	int i, j = 0, length = excludedItems_.size();
	for (i = 0; i < EXCLUDED_WIDTH; i++) {
		Slot *slot = excludedSlots_[i];

		if (j >= length) {
			slot->setItem( 0 );
		}
		else {
			Item *item = excludedItems_[j];
			slot->setItem( item );
			j++;
		}
	}
}

Slot* Inventory::getSlot( uint8 index )
{
	// Don't allow invalid slots.
	if (!isValidSlot( index ))
		throw Exception( "Attempted to get slot with invalid index." );

	return inventory_[index];
}

const slotArray Inventory::getInventory()
{
	return inventory_;
}

const slotArray Inventory::getExcluded()
{
	return excludedSlots_;
}

int Inventory::getCapacity() const
{
	return (pageWidth_ * pageHeight_ * pages_);
}

bool Inventory::isValidSlot ( uint8 index )
{
	return (index >= 0) && (index < getCapacity());
}