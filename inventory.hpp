#ifndef INVENTORY_HPP
#define INVENTORY_HPP

#include "dynamic_slot_book.hpp"
#include "item.hpp"
#include "item_schema.hpp"
#include "slot.hpp"
#include "slot_book.hpp"

/*
 * Class that manages an inventory book and fallback slots.
 *
 * The main invariant that this class aims to preserve is that
 * item positions must be the same as stored on the Steam backend.
 */
class Inventory
{

public:

	Inventory( SlotBook* book, DynamicSlotBook* excluded );
	virtual ~Inventory( void );

    // Slot book management.
    bool set_inventory_size( unsigned int slots );

	// Item query functions.
	Item* find_item( uint64 unique_id ) const;
    Item* get_inventory_item( unsigned int index ) const;
    Item* get_excluded_item( unsigned int index ) const;

    // Item addition/removal.
	bool add_item( Item* item );
    bool can_place( const Item* item ) const;
	bool place_item( Item* item, unsigned int index );
    void displace_item( Item* item );
    void remove_item( Item* item );
	void remove_items( void );
    void delete_items( void );

	// Excluded handling.
	bool resolve_excluded( void );

private:

	// Slot arrays.
	SlotBook* book_;
	DynamicSlotBook* excluded_;
    JUTIL::Vector<Item*> items_;

};

#endif // INVENTORY_H