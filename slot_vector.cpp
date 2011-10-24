#include "slot_vector.h"

SlotVector::SlotVector( unsigned int slots )
{
	add_slots( slots );
}

SlotVector::~SlotVector()
{
	remove_slots();
}

void SlotVector::add_slots( unsigned int slots )
{
	unsigned int startIndex = slots_.size();
	for (unsigned int i = 0; i < slots; ++i) {
		Slot* slot = new IndexSlot( startIndex + i );
		slots_.push_back( slot );
	}
}

void SlotVector::remove_slots()
{
	std::vector<Slot*>::iterator i;
	for (i = slots_.begin(); i != slots_.end(); i = slots_.erase( i )) {
		delete *i;
	}
}

Slot* SlotVector::get_slot_by_item( Item* item ) const
{
	// Check by index.
	uint16 index = item->get_index();
	if (is_valid_index( index )) {
		Slot* slot = get_slot_by_index( index );
		if (slot->get_item() == item) {
			return slot;
		}
	}

	return nullptr;
}

Slot* SlotVector::get_slot_by_index( unsigned int index ) const
{
	return slots_[index];
}

bool SlotVector::is_slot_empty( unsigned int index ) const
{
	return is_valid_index( index ) && (slots_[index]->get_item() == nullptr);
}

bool SlotVector::is_valid_index( unsigned int index ) const
{
	return index < slots_.size();
}

unsigned int SlotVector::get_slot_count() const
{
	return slots_.size();
}

void SlotVector::remove_item( Item* item )
{
	Slot* slot = get_slot_by_item( item );
	if (slot != nullptr) {
		slot->set_item( nullptr );
	}
}

void SlotVector::empty_slots()
{
	std::vector<Slot*>::const_iterator i, end;
	for (i = slots_.begin(), end = slots_.end(); i != end; ++i) {
		Slot* slot = *i;
		slot->set_item( nullptr );
	}
}