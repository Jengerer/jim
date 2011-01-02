#include "Inventory.h"

// Excluded capacity.
const int	EXCLUDED_WIDTH		= 5;

Inventory::Inventory( 
	int width, int height, 
	int pages )
{
	// Set dimensions.
	invWidth_ = width;
	invHeight_ = height;
	pages_ = pages;

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
	slotVector::iterator i;
	for (i = inventory_.begin(); i != inventory_.end(); i++) {
		Slot* slot = *i;
		slot->setItem( 0 );
	}

	for (i = excludedSlots_.begin(); i != excludedSlots_.end(); i++) {
		Slot* slot = *i;
		slot->setItem( 0 );
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
	while (!inventory_.empty()) {
		Slot* slot = inventory_.back();

		// Delete it.
		delete slot;
		inventory_.pop_back();
	}

	while (!excludedSlots_.empty()) {
		Slot* slot = excludedSlots_.back();

		// Delete it.
		delete slot;
		excludedSlots_.pop_back();
	}
}

void Inventory::generateSlots()
{
	// Create slots.
	for (int i = 0; i < getCapacity(); i++) {
		Slot *slot = new Slot( i );
		slot->setGroup( GROUP_INVENTORY );
		slot->setIndex( i );
		inventory_.push_back( slot );
	}

	// Create excluded slots.
	for (int i = 0; i < EXCLUDED_WIDTH; i++) {
		Slot *slot = new Slot();
		slot->setGroup( GROUP_EXCLUDED );
		excludedSlots_.push_back( slot );
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

Slot* Inventory::insert( Item* item ) {
	// Add item to correct slot.
	uint32 flags = item->getFlags();
	uint16 position = item->getIndex() - 1;
	if ((flags & 0xfff) && isValidSlot( position )) {
		Slot* destination = inventory_[position];

		// Should not allow overlap.
		if (destination->getItem() != 0) {
			// Check if slot free in excluded.
			slotVector::const_iterator i;
			for (i = excludedSlots_.begin(); i != excludedSlots_.end(); i++) {
				Slot *slot = *i;
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
		slotVector::const_iterator i;
		for (i = excludedSlots_.begin(); i != excludedSlots_.end(); i++) {
			Slot *slot = *i;
			if (slot->getItem() == 0) {
				slot->setItem( item );
				return slot;
			}
		}

		return 0;
	}
}

void Inventory::removeItem( Item* whichItem )
{
	// TODO: Find the item in inventory.
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

	// Move items.
	source->setItem( destination->getItem() );
	destination->setItem( target );
}

void Inventory::updateExcluded() {
	slotVector::iterator i;
	itemVector::iterator k = excludedItems_.begin();;
	for (i = excludedSlots_.begin(); i != excludedSlots_.end(); i++) {
		Slot* slot = *i;
		if (k != excludedItems_.end()) {
			slot->setItem( *k );
			k++;
		}
		else {
			slot->setItem( 0 );
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

const slotVector* Inventory::getInventory()
{
	return &inventory_;
}

const slotVector* Inventory::getExcluded()
{
	return &excludedSlots_;
}

int Inventory::getCapacity() const
{
	return (invWidth_ * invHeight_ * pages_);
}

bool Inventory::isValidSlot ( uint8 index )
{
	return (index >= 0) && (index < getCapacity());
}