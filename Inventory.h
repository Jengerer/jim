#pragma once

#include <vector>

#include "Item.h"
#include "Slot.h"

typedef Slot*	slotArray;
typedef vector<Slot*> slotVector;
typedef vector<Item*> itemVector;

class Inventory
{
public:
	Inventory(
		int width, int height, 
		int pages, int excludedWidth );
	virtual ~Inventory();

	int		GetWidth() const;
	int		GetHeight() const;
	int		GetPageCount() const;
	int		GetPageCapacity() const;
	int		GetInventorySize() const;
	int		GetExcludedSize() const;

	// Slot/item getters.
	Slot*		GetInventorySlot( int index );
	Slot*		GetExcludedSlot( int index );
	Item*		GetItemByUniqueId( uint64 id );

	const itemVector*	GetInventoryItems() const;
	const itemVector*	GetExcludedItems() const;

	// Slot resource functions.
	void	createSlots();
	void	AddSlots( unsigned int numSlots );
	void	removeSlots();
	void	emptySlots();

	// Get excluded page.
	int		GetExcludedPage() const;
	void	SetExcludedPage( int page );
	void	updateExcluded();

	// Item resource functions.
	Slot*	addItem( Item *item );
	Slot*	insertItem( Item *item );
	void	removeItem( Item *item );
	void	clearItems();

	// Item slot handling.
	void	moveItem( Slot *source, Slot *destination );
	bool	canMove( uint16 index );

private:

	// Dimension changes, should only be done within member functions.
	void	SetWidth( int width );
	void	SetHeight( int height );
	void	SetPageCount( int pageCount );

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