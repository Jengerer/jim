#ifndef SLOT_ARRAY_H
#define SLOT_ARRAY_H

#include "item.h"
#include "slot.h"
#include "slot_container.h"

/*
 * Class for maintaining a set of slots.
 */
class SlotArray : public SlotContainer
{

public:

	SlotArray( unsigned int size );
	~SlotArray( void );

	// Slot management functions.
	unsigned int get_slot_count( void ) const;

	// Slot getter.
	Slot*	get_slot( unsigned int index ) const;

private:

	// Slot management functions.
	void	set_size( unsigned int size );
	void	create_slots( void );
	void	destroy_slots( void );

private:

	Slot**			slots_;
	unsigned int	size_;

};

#endif // SLOT_ARRAY_H