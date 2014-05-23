#ifndef SLOT_ARRAY_INTERFACE_HPP
#define SLOT_ARRAY_INTERFACE_HPP

#include "slot.hpp"

/*
 * Interface used to abstract slot array implementation from
 * slot array representatives.
 */
class SlotArrayInterface
{

public:

	virtual const Slot* get_slot( unsigned int index ) const = 0;
	virtual unsigned int get_size( void ) const = 0;

};

#endif // SLOT_ARRAY_INTERFACE_HPP