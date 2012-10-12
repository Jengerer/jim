#ifndef INVENTORY_H
#define INVENTORY_H

#include <set>
#include "dynamic_slot_book.hpp"
#include "item.hpp"
#include "slot.hpp"
#include "slot_book.hpp"

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
	Item* find_item( uint64 unique_id ) const;
	bool insert_item( Item* item );
	void move_item( Item* item, unsigned int index );
	void remove_item( Item* item );
	void remove_items( void );
	bool can_insert( const Item* item ) const;
	bool is_excluded( const Item* item ) const;

	// Excluded handling.
	void resolve_excluded( void );

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
	std::set<Item*> selected_;

};

#endif // INVENTORY_H