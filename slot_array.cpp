#include "slot_array.h"

/*
 * Slot array constructor.
 */
SlotArray::SlotArray( unsigned int size )
{
	set_size( size );
	create_slots();
}

/*
 * Slot array destructor.
 */
SlotArray::~SlotArray( void )
{
	destroy_slots();
}

/*
 * Get number of slots.
 */
unsigned int SlotArray::get_slot_count( void ) const
{
	return size_;
}

/*
 * Get a slot at an index. Assumes index is valid.
 */
Slot* SlotArray::get_slot( unsigned int index ) const
{
	return slots_[index];
}

/*
 * Set array size.
 */
void SlotArray::set_size( unsigned int size )
{
	size_ = size;
}

/*
 * Create slots.
 */
void SlotArray::create_slots( void )
{
	unsigned int slot_count = get_slot_count();
	slots_ = new Slot*[ slot_count ];

	// Initialize slots.
	for (unsigned int i = 0; i < slot_count; i++) {
		slots_[i] = new Slot();
	}
}

/*
 * Destroy slots.
 */
void SlotArray::destroy_slots( void )
{
	// Destroy slots.
	for (unsigned int i = 0; i < get_slot_count(); i++) {
		delete slots_[i];
	}
	slots_ = nullptr;
}