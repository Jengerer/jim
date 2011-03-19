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
	int				getCapacity() const;

	// Get slot vector.
	const slotArray	getInventory();
	const slotArray	getExcluded();

	// Slot handling.
	void			generateSlots();
	virtual void	move( Slot *source, Slot *destination );
	bool			isValidSlot( uint8 index );
	Slot*			getSlot( uint8 index );

	// Item handling.
	Slot*			addItem( Item* item );
	itemVector*		getItems();
	Slot*			insert( Item* item );
	virtual void	removeItem( uint64 uniqueId );
	void			emptySlots();
	virtual void	clearItems();
	virtual void	clearSlots();
	void			updateExcluded();

protected:
	// Inventory attributes.
	int				pageWidth_, pageHeight_;
	int				pages_;

private:
	// Item vectors.
	itemVector	items_;
	itemVector	excludedItems_;

	// Slot arrays.
	slotArray	inventory_;
	slotArray	excludedSlots_;
};