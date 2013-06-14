#ifndef SLOT_ARRAY_HPP
#define SLOT_ARRAY_HPP

#include "item.hpp"
#include "slot.hpp"
#include "slot_array_listener.hpp"
#include <containers/vector.hpp>

/*
 * Class for maintaining a set of slots.
 */
class SlotArray
{

public:

	SlotArray( void );
	~SlotArray( void );

	// Slot management functions.
    bool set_size( unsigned int size );
	unsigned int get_size( void ) const;

	// Item management.
	bool has_item( Item* item ) const;
	void set_item( unsigned int index, Item* item );
	const Item* get_item( unsigned int index ) const;

private:

	// Slot management functions.
	void destroy_slots( void );

protected:

	SlotArrayListener* listener_;
	JUTIL::Vector<Slot*> slots_;

private:

	JUTIL::Map<Item*, unsigned int> positions_;

};

#endif // SLOT_ARRAY_HPP