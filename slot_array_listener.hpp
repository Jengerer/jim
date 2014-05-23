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
	virtual void on_slot_updated( unsigned int index, const Slot* slot ) = 0;

};

#endif // SLOT_ARRAY_LISTENER_HPP