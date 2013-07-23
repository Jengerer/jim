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
    bool is_valid_index( unsigned int index ) const;

	// Item management.
	void set_item( unsigned int index, Item* item );
	Item* get_item( unsigned int index );
    bool remove_item( Item* item );
    void empty_slots( void );

private:

	// Slot management functions.
	void destroy_slots( void );

protected:

    JUTIL::Vector<Slot> slots_;
	SlotArrayListener* listener_;

};

#endif // SLOT_ARRAY_HPP