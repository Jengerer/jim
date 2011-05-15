#pragma once

#include <vector>
#include <map>

#include "Item.h"
#include "Slot.h"

typedef Slot* slotArray;
typedef vector<Slot*> slotVector;
typedef pair<const uint64, Item*> itemPair;
typedef map<uint64, Item*> itemMap;

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
	Slot*		GetInventorySlot( int index ) const;
	Slot*		GetExcludedSlot( int index ) const;
	Item*		GetItemByUniqueId( uint64 id );

	const itemMap*	GetInventoryItems( void ) const;
	const itemMap*	GetExcludedItems( void ) const;

	// Slot resource functions.
	void	CreateSlots( void );
	void	AddSlots( unsigned int numSlots );
	void	RemoveSlots( void );
	void	EmptySlots( void );

	// Get excluded page.
	int		GetExcludedPage( void ) const;
	void	SetExcludedPage( int page );
	void	UpdateExcluded( void );
	void	ResolveExcluded( void );

	// Item resource functions.
	void	InsertItem( Item *item );
	void	RemoveItem( uint64 uniqueId );
	void	ClearItems( void );

	// Item list maintenance.
	void	ToInventory( Item *item );
	void	ToExcluded( Item *item );

	// Item slot handling.
	void	MoveItem( Slot *source, Slot *destination );
	bool	IsValidIndex( uint16 index ) const;
	bool	CanMove( uint16 index ) const;

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
	itemMap		inventoryItems_;
	itemMap		excludedItems_;

	// Slot arrays.
	slotArray	inventorySlots_;
	slotArray	excludedSlots_;
};