#ifndef SLOT_VECTOR_H
#define SLOT_VECTOR_H

#include "index_slot.h"
#include "slot.h"
#include <vector>

class SlotVector
{
public:

	SlotVector( unsigned int slots );
	~SlotVector();

	// Managing slots.
	void add_slots( unsigned int slots );
	void remove_slots( void );

	// Getting slots.
	Slot* get_slot_by_item( Item* item ) const;
	Slot* get_slot_by_index( unsigned int index ) const;

	// Index and size comparison.
	bool is_slot_empty( unsigned int index ) const;
	bool is_valid_index( unsigned int index ) const;
	unsigned int get_slot_count( void ) const;

	// Managing items.
	void remove_item( Item* item );
	void empty_slots();

private:

	std::vector<Slot*> slots_;

};

#endif // SLOT_VECTOR_H