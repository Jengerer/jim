#include "Inventory.h"

Inventory::Inventory( 
	int width, int height, 
	int pages )
{
	// Set dimensions.
	width_ = width;
	height_ = height;
	pages_ = pages;

	createSlots();
}

Inventory::~Inventory()
{
	// Inventory has been deleted.
	clearSlots();
	clearItems();
}

void Inventory::clearItems()
{
	// First remove items from slots.
	slotVector::iterator i;
	for (i = inventory_.begin(); i != inventory_.end(); i++) {
		Slot* slot = *i;
		if (slot->item)
			slot->item = 0;
	}

	for (i = excluded_.begin(); i != excluded_.end(); i++) {
		Slot* slot = *i;
		if (slot->item)
			slot->item = 0;
	}

	// Now delete all items.
	itemVector::iterator j;
	while (!items_.empty()) {
		// Get item.
		j = items_.begin();
		Item* item = *j;

		// Delete it.
		delete item;
		item = 0;

		// Remove from vector.
		items_.erase( j );
	}
}

void Inventory::createSlots()
{
	// Create slots.
	for (int i = 0; i < getCapacity(); i++) {
		Slot* slot = new Slot();
		slot->index = i;

		// Add to inventory.
		inventory_.push_back( slot );
	}
}

void Inventory::clearSlots()
{
	// Iterator for slots.
	slotVector::iterator i;

	// Delete inventory.
	for (i = inventory_.begin(); i != inventory_.end(); i++)
		delete *i;

	// Delete excluded.
	for (i = excluded_.begin(); i != excluded_.end(); i++)
		delete *i;

	// Clear vectors.
	inventory_.clear();
	excluded_.clear();
}

void Inventory::add( Item* item )
{
	// Add to list.
	items_.push_back( item );

	// Add item to correct slot.
	uint8 position = item->getPosition() - 1;
	if (isValidSlot( position )) {
		Slot* destSlot = inventory_[position];

		// Should not allow overlap.
		if (destSlot->item) {
			item->setGroup( GROUP_EXCLUDED );
			excluded_.push_back( new Slot( item ) );
		}
		else {
			destSlot->item = item;
		}
	}
	else {
		item->setGroup( GROUP_EXCLUDED );
		excluded_.push_back( new Slot( item ) );
	}
}

void Inventory::remove( Item* whichItem )
{
	// TODO: Find the item in inventory.
}

void Inventory::move( Slot* source, Slot* destination )
{
	// Get item we're moving.
	Item* item1 = source->item;

	if (!destination->item)
	{
		destination->item = item1;
		source->item = 0;
	}
	else {
		Item* tempItem = item1;
		source->item = destination->item;
		destination->item = tempItem;
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
	return &excluded_;
}

int Inventory::getCapacity() const
{
	return (width_ * height_ * pages_);
}

bool Inventory::isValidSlot ( uint8 index )
{
	return (index >= 0) && (index < getCapacity());
}