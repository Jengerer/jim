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
	slots_.get_length();
}

/*
 * Set the item at the given slot.
 */
void SlotArray::set_item( unsigned int index, Item* item )
{
	Slot* slot = &slots_.at( index );
	slot->set_item( item );
}

/*
 * Get the item at the given slot.
 */
Item* SlotArray::get_item( unsigned int index )
{
	Slot* slot = &slots_.at( index );
	return slot->get_item();
}
 
/*
 * Destroy slots.
 */
void SlotArray::destroy_slots( void )
{
	// Destroy slots.
    slots_.clear();
}