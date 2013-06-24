#ifndef SLOT_ARRAY_LISTENER_HPP
#define SLOT_ARRAY_LISTENER_HPP

#include "slot.hpp"

/*
 * Interface for updating a representation of a slot array.
 */
class SlotArrayListener
{

public:

	// Triggers when the item contained in a slot changes.
	virtual bool on_slot_updated( unsigned int index, Slot* slot ) = 0;

};

#endif // SLOT_ARRAY_LISTENER_HPP