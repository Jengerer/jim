#ifndef SLOT_ARRAY_H
#define SLOT_ARRAY_H

#include "item.hpp"
#include "slot.hpp"
#include "slot_container.hpp"
#include <containers/vector.hpp>

/*
 * Class for maintaining a set of slots.
 */
class SlotArray : public SlotContainer
{

public:

	SlotArray( unsigned int size, unsigned int start_index = 0 );
	~SlotArray( void );

	// Slot management functions.
    bool initialize( void );
	unsigned int get_slot_count( void ) const;

	// Slot getter.
	Slot* get_slot( unsigned int index ) const;

private:

	// Slot management functions.
	void set_size( unsigned int size );
	bool create_slots( unsigned int start_index );
	void destroy_slots( void );

private:

	JUTIL::Vector<Slot*> slots_;
	unsigned int	size_;
    unsigned int    start_index_;

};

#endif // SLOT_ARRAY_H