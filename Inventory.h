#pragma once

#include <vector>

#include "Item.h"
#include "Slot.h"

#define EXCLUDED_WIDTH 5

typedef Slot**	slotArray;
typedef vector<Slot*> slotVector;
typedef vector<Item*> itemVector;

class Inventory
{
public:
	Inventory(
		int width, int height, 
		int pages );
	virtual ~Inventory();

	// Dimension getters.
	int					getCapacity() const;

	// Get slot vector.
	const slotArray	getInventory();
	const slotArray	getExcluded();

	// Slot handling.
	void			generateSlots();
	void			move( Slot *source, Slot *destination );
	bool			isValidSlot( uint8 index );
	Slot*			getSlot( uint8 index );

	// Item handling.
	Slot*			addItem( Item* item );
	Slot*			insert( Item* item );
	void			removeItem( Item* item );
	void			emptySlots();
	void			clearItems();
	void			clearSlots();
	void			updateExcluded();

protected:
	// Inventory attributes.
	int				invWidth_, invHeight_;
	int				pages_;

private:
	// Item vectors.
	itemVector	items_;
	itemVector	excludedItems_;

	// Slot arrays.
	slotArray	inventory_;
	slotArray	excludedSlots_;
};