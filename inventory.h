#ifndef INVENTORY_H
#define INVENTORY_H

#include <set>
#include "dynamic_slot_book.h"
#include "item.h"
#include "slot.h"
#include "slot_book.h"

/*
 * Class that manages an inventory book and fallback slots.
 */
class Inventory
{
public:

	Inventory( SlotBook* book, DynamicSlotBook* excluded );
	virtual ~Inventory( void );

	// Slot vector getters.
	const SlotBook* get_book( void ) const;
	const DynamicSlotBook* get_excluded_book( void ) const;

	// Item handling.
	Item*	find_item( uint64 unique_id ) const;
	void	insert_item( Item* item );
	void	remove_item( Item* item );
	void	remove_items();
	bool	can_insert( const Item* item ) const;

	// Excluded handling.
	void	resolve_excluded( void );

private:

	// Slot book setters.
	void set_book( SlotBook* book );
	void set_excluded_book( DynamicSlotBook* excluded_book );

protected:

	// Slot arrays.
	SlotBook* book_;
	DynamicSlotBook* excluded_book_;

private:

	// Item vectors.
	std::set<Item*> items_;

};

#endif // INVENTORY_H