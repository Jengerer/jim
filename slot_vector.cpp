#include "slot_vector.h"

SlotVector::SlotVector( unsigned int slots )
{
	AddSlots( slots );
}

SlotVector::~SlotVector()
{
	RemoveSlots();
}

void SlotVector::AddSlots( unsigned int slots )
{
	unsigned int startIndex = slots_.size();
	for (unsigned int i = 0; i < slots; ++i) {
		Slot* slot = new Slot( startIndex + i );
		slots_.push_back( slot );
	}
}

void SlotVector::RemoveSlots()
{
	vector<Slot*>::iterator i;
	for (i = slots_.begin(); i != slots_.end(); i = slots_.erase( i )) {
		delete *i;
	}
}

Slot* SlotVector::GetSlotByItem( Item* item ) const
{
	vector<Slot*>::const_iterator i, end;
	for (i = slots_.begin(), end = slots_.end(); i != end; ++i) {
		Slot* slot = *i;
		if (slot->GetItem() == item) {
			return slot;
		}
	}

	return nullptr;
}

Slot* SlotVector::GetSlotByIndex( unsigned int index ) const
{
	return slots_[index];
}

bool SlotVector::IsSlotEmpty( unsigned int index ) const
{
	return IsValidIndex( index ) && (slots_[index]->GetItem() == nullptr);
}

bool SlotVector::IsValidIndex( unsigned int index ) const
{
	return index < slots_.size();
}

unsigned int SlotVector::GetSlotCount() const
{
	return slots_.size();
}

void SlotVector::RemoveItem( Item* item )
{
	vector<Slot*>::const_iterator i, end;
	for (i = slots_.begin(), end = slots_.end(); i != end; ++i) {
		Slot* slot = *i;
		if (slot->GetItem() == item) {
			slot->SetItem( nullptr );
			return;
		}
	}
}

void SlotVector::EmptySlots()
{
	vector<Slot*>::const_iterator i, end;
	for (i = slots_.begin(), end = slots_.end(); i != end; ++i) {
		Slot* slot = *i;
		slot->SetItem( nullptr );
	}
}

void SlotVector::Move( Slot* source, Slot* dest )
{
	// Get items.
	Item* sourceItem = source->GetItem();
	Item* destItem = dest->GetItem();

	// Switch indices.
	destItem->SetIndex( source->GetIndex() );
	sourceItem->SetIndex( dest->GetIndex() );

	// Move to slots.
	source->SetItem( destItem );
	dest->SetItem( sourceItem );
}