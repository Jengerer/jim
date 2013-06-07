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
	destroy_slots();
}

/*
 * Set slot array size.
 */
bool SlotArray::set_size( unsigned int size )
{
	unsigned int i;
	unsigned int old_size = get_size();

	// Delete slots if shrinking.
	if (size < old_size) {
		for (i = size; i < old_size; ++i) {
			Slot* slot = slots_.at( i );
			JUTIL::BaseAllocator::destroy( slot );
		}
		
		if (!slots_.resize( size )) {
			return false;
		}
	}
	else {
		// Add new slots.
		if (!slots_.reserve( size )) {
			return false;
		}

		// Create new slots if expanding.
		for (unsigned int i = old_size; i < size; ++i) {
			Slot* slot;
			if (!JUTIL::BaseAllocator::allocate( &slot )) {
				return false;
			}

			slot = new (slot) Slot();
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
	slots_.get_length();
}

/*
 * Set the item at the given slot.
 */
void SlotArray::set_item( unsigned int index, Item* item )
{
}

/*
 * Destroy slots.
 */
void SlotArray::destroy_slots( void )
{
	// Destroy slots.
    size_t i;
    size_t length = get_size();
	for (i = 0; i < length; ++i) {
        Slot* slot = slots_.at( i );
        JUTIL::BaseAllocator::destroy( slot );
	}
    slots_.clear();
}