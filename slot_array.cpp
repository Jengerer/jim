#include "slot_array.hpp"

/*
 * Slot array constructor.
 */
SlotArray::SlotArray( unsigned int size, unsigned int start_index )
{
    start_index_ = start_index;
	set_size( size );
}

/*
 * Slot array destructor.
 */
SlotArray::~SlotArray( void )
{
	destroy_slots();
}

/*
 * Initialize slot array.
 */
bool SlotArray::initialize( void )
{
    return create_slots( start_index_ );
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
	return slots_.get( index );
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
bool SlotArray::create_slots( unsigned int start_index )
{
	unsigned int slot_count = get_slot_count();
    if (!slots_.set_length( slot_count )) {
        return false;
    }

	// Initialize slots.
    bool success = true;
	for (unsigned int i = 0; i < slot_count; i++) {
        Slot* slot;
        if (!JUTIL::BaseAllocator::allocate( &slot )) {
            success = false;
            break;
        }
        slot = new (slot) Slot( i + start_index );
        slots_.set( i, slot );
	}

    // Clean if failed.
    if (!success) {
        slots_.clear();
    }
    return success;
}

/*
 * Destroy slots.
 */
void SlotArray::destroy_slots( void )
{
	// Destroy slots.
    size_t i;
    size_t length = get_slot_count();
	for (i = 0; i < length; ++i) {
        Slot* slot = slots_.get( i );
        JUTIL::BaseAllocator::destroy( slot );
	}
    slots_.clear();
}