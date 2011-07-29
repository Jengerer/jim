#pragma once

#include <set>

#include "item.h"
#include "slot.h"
#include "slot_vector.h"

// Compare class for items by unique ID.
class item_cmp
{
public:
	bool operator()( const Item* item1, const Item* item2 )
	{
		return item1->GetUniqueId() < item2->GetUniqueId();
	}
};

class Inventory
{
public:

	Inventory( unsigned int inventorySize, unsigned int excludedSize );
	virtual ~Inventory();

	// Size getters.
	unsigned int GetInventorySize() const;
	unsigned int GetExcludedSize() const;

	// Item handling.
	Item*	GetItemByUniqueId( uint64 uniqueId );
	Slot*	InsertItem( Item* item );
	void	RemoveItem( Item* item );
	void	RemoveItems();
	bool	CanInsert( const Item* item ) const;

	// Slot resource functions.
	void	AddSlots( unsigned int slots );
	void	RemoveSlots();
	void	EmptySlots();

	// Handling excluded items.
	void	SetExcludedPage( unsigned int excludedPage );
	void	UpdateExcluded( void );
	void	ResolveExcluded( void );

	// Moving from different sets.
	void	ToInventory( Item* item );
	void	ToExcluded( Item* item );

private:

	void	CreateSlots();

protected:

	// Slot arrays.
	SlotVector*		inventorySlots_;
	SlotVector*		excludedSlots_;

private:

	// Inventory attributes.
	unsigned int inventorySize_;
	unsigned int excludedSize_;

	// Scrolling through excluded.
	unsigned int excludedPage_;

	// Item vectors.
	std::set<Item*, item_cmp> inventoryItems_;
	std::set<Item*, item_cmp> excludedItems_;

};