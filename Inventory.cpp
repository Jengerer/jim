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
	// Iterate and delete.
	vector<Item*>::iterator iter;
	while (!items_.empty()) {
		// Get this item.
		iter = items_.begin();
		Item* item = *iter;

		// Delete item.
		delete item;
		item = NULL;

		// Remove from vector.
		items_.erase(iter);
	}
}

void Inventory::createSlots()
{
	// Create slots.
	for (int i = 0; i < getCapacity(); i++) {
		Slot* slot = new Slot();

		// Calculate the real position of this slot.
		int xIndex = i % width_;
		int yIndex = i / width_;

		// Add to inventory.
		inventory_.push_back( slot );
	}
}

void Inventory::clearSlots()
{
	// Iterator for slots.
	vector<Slot*>::iterator i;

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