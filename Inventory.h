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

	int		GetWidth( void ) const;
	int		GetHeight( void ) const;
	int		GetPageCount( void ) const;
	int		GetPageSize( void ) const;
	int		GetInventorySize( void ) const;
	int		GetExcludedSize( void ) const;

	// Slot/item getters.
	Slot*		GetInventorySlot( int index );
	Slot*		GetExcludedSlot( int index );
	Item*		GetItemByUniqueId( uint64 id );

	const itemVector*	GetInventoryItems( void ) const;
	const itemVector*	GetExcludedItems( void ) const;

	// Slot resource functions.
	void	CreateSlots( void );
	void	AddSlots( unsigned int numSlots );
	void	RemoveSlots( void );
	void	EmptySlots( void );

	// Get excluded page.
	int		GetExcludedPage( void ) const;
	void	SetExcludedPage( int page );
	void	UpdateExcluded( void );

	// Item resource functions.
	Slot*	AddItem( Item *item );
	Slot*	InsertItem( Item *item );
	void	RemoveItem( Item *item );
	void	ClearItems( void );

	// Item slot handling.
	void	MoveItem( Slot *source, Slot *destination );
	bool	CanMove( uint16 index );

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