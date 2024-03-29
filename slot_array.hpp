#ifndef SLOT_ARRAY_HPP
#define SLOT_ARRAY_HPP

#include "item.hpp"
#include "slot.hpp"
#include "slot_array_interface.hpp"
#include "slot_array_listener.hpp"
#include <containers/vector.hpp>

/*
 * Class for maintaining a set of slots.
 */
class SlotArray : public SlotArrayInterface
{

public:

	SlotArray( void );
	~SlotArray( void );

	// Slot array listener and interface functions.
	virtual const Slot* get_slot( unsigned int index ) const;
	virtual unsigned int get_size( void ) const;
	void set_listener( SlotArrayListener* listener );

	// Slot management functions.
    bool set_size( unsigned int size );
    bool is_valid_index( unsigned int index ) const;

	// Item and slot query functions.
    bool is_slot_empty( unsigned int index ) const;
    bool contains_item( Item* item, unsigned int* index = nullptr ) const;
    Item* get_item( unsigned int index ) const;

    // Slot state functions.
	void set_item( unsigned int index, Item* item );
    void set_enabled( unsigned int index, bool is_enabled );
	void set_enabled( bool is_enabled );
    bool remove_item( Item* item );
	void update_slot( unsigned int index );
    void empty_slots( void );
	void destroy_slots( void );

protected:

    JUTIL::Vector<Slot*> slots_;
	SlotArrayListener* listener_;

};

#endif // SLOT_ARRAY_HPP