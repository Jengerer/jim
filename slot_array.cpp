#include "slot_array.hpp"

/*
 * Slot array constructor.
 */
SlotArray::SlotArray( void ) :
	slots_(),
	listener_( nullptr )
{
}

/*
 * Slot array destructor.
 */
SlotArray::~SlotArray( void )
{
	destroy_slots();
}

/*
 * Get slot by index.
 */
const Slot* SlotArray::get_slot( unsigned int index ) const
{
	return slots_.at( index );
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

	// Don't do anything if no change.
	if (size == old_size) {
		return true;
	}

	// Delete slots if shrinking.
    Slot* slot;
    if (size < old_size) {
		// Empty slots and update before deleting.
		for (i = size; i < old_size; ++i) {
			set_item( i, nullptr );
		}

		// Now destroy.
        for (i = size; i < old_size; ++i) {
            slot = slots_.at( i );
            JUTIL::BaseAllocator::destroy( slot );
        }

		// Change array size.
		if (size == 0) {
			slots_.clear();
		}
		else {
			slots_.resize( size );
		}
    }
    else {
        // Reserve space for new slots.
        if (!slots_.reserve( size )) {
            return false;
        }
        // Push slots into reserved space.
        for (i = old_size; i < size; ++i) {
            if (!JUTIL::BaseAllocator::allocate( &slot )) {
                return false;
            }
            new (slot) Slot();
            slots_.push( slot );
        }
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
void SlotArray::set_item( unsigned int index, Item* item )
{
	Slot* slot = slots_.at( index );
	slot->set_item( item );
	
	// Reset select state on removal.
	if (item == nullptr) {
		slot->set_enabled( true );
	}
	listener_->on_slot_updated( index, slot );
}

/*
 * Set the slot as enabled.
 */
void SlotArray::set_enabled( unsigned int index, bool is_enabled )
{
    Slot* slot = slots_.at( index );
    slot->set_enabled( is_enabled );
    listener_->on_slot_updated( index, slot );
}

/*
 * Set all slots' enabled status.
 */
void SlotArray::set_enabled( bool is_enabled )
{
	unsigned int i;
	unsigned int end = get_size();
	for (i = 0; i < end; ++i) {
		set_enabled( i, is_enabled );
	}
}

/*
 * Get the item at the given slot.
 */
Item* SlotArray::get_item( unsigned int index ) const
{
	const Slot* slot = get_slot( index );
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
 * Push item update about a slot.
 */
void SlotArray::update_slot( unsigned int index )
{
	const Slot* slot = get_slot( index );
	listener_->on_slot_updated( index, slot );
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
	unsigned int i;
	unsigned int end = get_size();
	for (i = 0; i < end; ++i) {
		Slot* slot = slots_.at( i );
		JUTIL::BaseAllocator::destroy( slot );
	}
	slots_.clear();
}