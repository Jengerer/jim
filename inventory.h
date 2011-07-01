#pragma once

#include <vector>

#include "item.h"
#include "slot.h"
#include "slot_vector.h"

class Inventory
{
public:

	Inventory( unsigned int inventorySize, unsigned int excludedSize );
	virtual ~Inventory();

	// Size getters.
	unsigned int GetInventorySize() const;
	unsigned int GetExcludedSize() const;

	// Item handling.
	Item*	GetItemByUniqueId( uint64 uniqueId, bool shouldRemove = false );
	void	InsertItem( Item* item );
	void	RemoveItem( uint64 uniqueId );
	void	RemoveItems();

	// Slot resource functions.
	void	AddSlots( unsigned int slots );
	void	EmptySlots();

	// Handling excluded items.
	void	SetExcludedPage( unsigned int excludedPage );
	void	UpdateExcluded( void );
	void	ResolveExcluded( void );

	// Item list maintenance.
	void	ToInventory( Item *item );
	void	ToExcluded( Item *item );

private:

	void	CreateSlots();
	void	RemoveSlots();

protected:

	// Slot arrays.
	SlotVector*		inventory_;
	SlotVector*		excluded_;

private:

	// Inventory attributes.
	unsigned int	inventorySize_;
	unsigned int	excludedSize_;

	// Scrolling through excluded.
	unsigned int	excludedPage_;

	// Item vectors.
	vector<Item*>	inventoryItems_;
	vector<Item*>	excludedItems_;

};