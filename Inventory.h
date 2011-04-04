#pragma once

#include <vector>

#include "Item.h"
#include "Slot.h"

typedef Slot**	slotArray;
typedef vector<Slot*> slotVector;
typedef vector<Item*> itemVector;

class Inventory
{
public:
	Inventory(
		int width, int height, 
		int pages, int excludedWidth );
	virtual ~Inventory();

	// Dimension getters.
	int		getWidth() const;
	int		getHeight() const;
	int		getPages() const;
	int		inventorySize() const;
	int		excludedSize() const;

	// Slot/item getters.
	Slot*		getInventorySlot( int index );
	Slot*		getExcludedSlot( int index );
	Item*		getItem( uint64 id );
	itemVector*	getInventoryItems();
	itemVector* getExcludedItems();

	// Slot resource functions.
	void	createSlots();
	void	removeSlots();
	void	emptySlots();

	// Get excluded page.
	int		getExcludedPage() const;
	void	setExcludedPage( int page );
	void	updateExcluded();

	// Item resource functions.
	Slot*	addItem( Item *item );
	Slot*	insertItem( Item *item );
	void	removeItem( Item *item );
	void	clearItems();

	// Item slot handling.
	void	moveItem( Slot *source, Slot *destination );
	bool	canMove( uint8 index );

private:
	// Inventory attributes.
	int		width_, height_, pages_;
	int		excludedWidth_;

	// Excluded page view.
	int		excludedPage_;

	// Item vectors.
	itemVector	inventoryItems_;
	itemVector	excludedItems_;

	// Slot arrays.
	slotArray	inventorySlots_;
	slotArray	excludedSlots_;
};