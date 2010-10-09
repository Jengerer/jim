#pragma once

#include <vector>

#include "Item.h"
#include "Slot.h"

typedef vector<Slot*> slotVector;
typedef vector<Item*> itemVector;

class Inventory
{
public:
	Inventory(
		int width, int height, 
		int pages );
	~Inventory();

	// Dimension getters.
	int					getCapacity() const;

	// Get slot vector.
	const slotVector*	getInventory();
	const slotVector*	getExcluded();

	// Slot handling.
	void			createSlots();
	void			clearSlots();
	void			move( Slot* slot1, Slot* slot2 );
	bool			isValidSlot( uint8 index );
	Slot*			getSlot( uint8 index );

	// Item handling.
	void			add( Item* item );
	bool			insert( Item* item );
	void			remove( Item* item );
	void			clearItems();

protected:
	// Inventory attributes.
	int				width_, height_;
	int				pages_;

private:
	// Slot vectors.
	itemVector	items_;
	slotVector	inventory_;
	slotVector	excluded_;
};
