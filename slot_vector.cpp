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
		Slot* slot = new IndexSlot( startIndex + i );
		slots_.push_back( slot );
	}
}

void SlotVector::RemoveSlots()
{
	std::vector<Slot*>::iterator i;
	for (i = slots_.begin(); i != slots_.end(); i = slots_.erase( i )) {
		delete *i;
	}
}

Slot* SlotVector::GetSlotByItem( Item* item ) const
{
	// Check by index.
	uint16 index = item->GetIndex();
	if (IsValidIndex( index )) {
		Slot* slot = GetSlotByIndex( index );
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
	Slot* slot = GetSlotByItem( item );
	if (slot != nullptr) {
		slot->SetItem( nullptr );
	}
}

void SlotVector::EmptySlots()
{
	std::vector<Slot*>::const_iterator i, end;
	for (i = slots_.begin(), end = slots_.end(); i != end; ++i) {
		Slot* slot = *i;
		slot->SetItem( nullptr );
	}
}