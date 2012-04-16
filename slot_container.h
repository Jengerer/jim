#ifndef SLOT_CONTAINER_H
#define SLOT_CONTAINER_H

#include "slot.h"

/*
 * Abstract class for maintaining slots.
 */
class SlotContainer
{

public:

	// Defined methods.
	bool is_empty( void ) const;
	bool is_slot_empty( unsigned int index ) const;
	bool is_valid_index( unsigned int index ) const;
	bool has_item( const Item* item ) const;
	virtual void insert_item( Item* item, unsigned int index ) const;
	virtual void remove_item( Item* item ) const;
	virtual void empty_slots( void ) const;

	// Methods to over-ride.
	virtual unsigned int get_slot_count( void ) const = 0;
	virtual Slot* get_slot( unsigned int index ) const = 0;

};

#endif // SLOT_CONTAINER_H