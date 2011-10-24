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
		return item1->get_unique_id() < item2->get_unique_id();
	}
};

class Inventory
{
public:

	Inventory( unsigned int inventorySize, unsigned int excludedSize );
	virtual ~Inventory();

	// Slot vector getters.
	const SlotVector* get_inventory_slots() const;
	const SlotVector* get_excluded_slots() const;

	// Size getters.
	unsigned int GetInventorySize() const;
	unsigned int GetExcludedSize() const;

	// Item handling.
	Item*	GetItemByUniqueId( uint64 uniqueId );
	Slot*	insert_item( Item* item );
	void	remove_item( Item* item );
	void	remove_items();
	bool	CanInsert( const Item* item ) const;

	// Slot resource functions.
	void	add_slots( unsigned int slots );
	void	remove_slots();
	void	empty_slots();

	// Handling excluded items.
	void	SetExcludedPage( unsigned int excludedPage );
	void	update_excluded( void );
	void	resolve_excluded( void );

	// Moving from different sets.
	void	ToInventory( Item* item );
	void	ToExcluded( Item* item );

protected:

	// Slot arrays.
	SlotVector*		inventorySlots_;
	SlotVector*		excludedSlots_;

private:

	// Scrolling through excluded.
	unsigned int excludedPage_;

	// Item vectors.
	std::set<Item*, item_cmp> inventoryItems_;
	std::set<Item*, item_cmp> excludedItems_;

};