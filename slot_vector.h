#ifndef SLOT_VECTOR_H
#define SLOT_VECTOR_H

#include "slot.h"
#include <vector>

class SlotVector
{
public:

	SlotVector( unsigned int slots );
	~SlotVector();

	// Managing slots.
	void AddSlots( unsigned int slots );
	void RemoveSlots();

	// Getting slots.
	Slot* GetSlotByItem( Item* item ) const;
	Slot* GetSlotByIndex( unsigned int index ) const;

	// Index and size comparison.
	bool IsSlotEmpty( unsigned int index ) const;
	bool IsValidIndex( unsigned int index ) const;
	unsigned int GetSlotCount() const;

	// Managing items.
	void Move( Slot* source, Slot* dest );
	void RemoveItem( Item* item );
	void EmptySlots();

private:

	std::vector<Slot*> slots_;

};

#endif // SLOT_VECTOR_H