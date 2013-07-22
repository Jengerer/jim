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

	Inventory( void );
	virtual ~Inventory( void );

    // Slot book primary and secondary initialization.
    bool initialize( void );
    bool set_inventory_size( unsigned int slots );

	// Slot book getters.
	SlotBook* get_book( void );
	DynamicSlotBook* get_excluded_book( void );

	// Inventory item query functions.
	Item* find_item( uint64 unique_id ) const;
    bool can_place( const Item* item ) const;
	bool is_excluded( const Item* item ) const;

    // Item addition/removal.
	bool add_item( Item* item );
	bool place_item( Item* item, unsigned int index );
    void remove_item( Item* item );
    void delete_item( Item* item );
	void delete_items( void );

	// Excluded handling.
	bool resolve_excluded( void );

protected:

	// Slot arrays.
	SlotBook book_;
	DynamicSlotBook excluded_book_;

};

#endif // INVENTORY_H