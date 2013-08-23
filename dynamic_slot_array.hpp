#ifndef DYNAMIC_SLOT_ARRAY_HPP
#define DYNAMIC_SLOT_ARRAY_HPP

#include "slot_array.hpp"

class DynamicSlotArray : public SlotArray
{

public:

	DynamicSlotArray( void );

	// Dynamic slot management.
    bool push_item( Item* item );
	bool compress_slots( void );

};

#endif // DYNAMIC_SLOT_ARRAY_HPP