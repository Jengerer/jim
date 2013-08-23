#include "slot_array.hpp"

/*
 * Slot array constructor.
 */
SlotArray::SlotArray( void )
{
}

/*
 * Slot array destructor.
 */
SlotArray::~SlotArray( void )
{
}

/*
 * Get slot by index.
 */
const Slot* SlotArray::get_slot( unsigned int index ) const
{
	return &slots_.at( index );
}

/*
 * Set handler for slot update events.
 */
void SlotArray::set_listener( SlotArrayListener* listener )
{
	listener_ = listener;
}

/*
 * Set slot array size.
 */
bool SlotArray::set_size( unsigned int size )
{
	unsigned int i;
	unsigned int old_size = get_size();

	// Delete slots if shrinking.
	if (!slots_.resize( size )) {
		return false;
	}

    // Construct new slots.
    for (i = old_size; i < size; ++i) {
        Slot* slot = &slots_.at( i );
        new (slot) Slot();
    }

	return true;
}

/*
 * Get number of slots.
 */
unsigned int SlotArray::get_size( void ) const
{
	return slots_.get_length();
}

/*
 * Returns whether the index corresponds to a valid slot.
 */
bool SlotArray::is_valid_index( unsigned int index ) const
{
    return index < get_size();
}

/*
 * Return whether a slot has no item.
 */
bool SlotArray::is_slot_empty( unsigned int index ) const
{
    return get_item( index ) == nullptr;
}

/*
 * Returns whether the slot array contains an item, and optionally fills out
 * the index it's contained at.
 */
bool SlotArray::contains_item( Item* item, unsigned int* index ) const
{
    unsigned int i;
    unsigned int end = get_size();
    for (i = 0; i < end; ++i) {
        Item* current = get_item( i );
        if (item == current) {
            if (index != nullptr) {
                *index = i;
            }
            return true;
        }
    }

    return false;
}

/*
 * Set the item at the given slot.
 */
bool SlotArray::set_item( unsigned int index, Item* item )
{
	Slot* slot = &slots_.at( index );
	slot->set_item( item );
	if (!listener_->on_slot_updated( index, slot )) {
		return false;
	}
	return true;
}

/*
 * Get the item at the given slot.
 */
Item* SlotArray::get_item( unsigned int index ) const
{
	const Slot* slot = &slots_.at( index );
	return slot->get_item();
}

/*
 * Remove an item from the inventory if it exists.
 * Returns true if it existed, false otherwise.
 */
bool SlotArray::remove_item( Item* item )
{
    unsigned int i;
    unsigned int end = get_size();
    for (i = 0; i < end; ++i) {
        Item* other = get_item( i );
        if (item == other) {
            set_item( i, nullptr );
            return true;
        }
    }

    return false;
}

/*
 * Empty the slots in the array.
 */
void SlotArray::empty_slots( void )
{
    unsigned int i;
    unsigned int end = get_size();
    for (i = 0; i < end; ++i) {
        set_item( i, nullptr );
    }
}
 
/*
 * Destroy slots.
 */
void SlotArray::destroy_slots( void )
{
	// Destroy slots.
    slots_.clear();
}