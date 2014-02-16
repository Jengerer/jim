#ifndef INVENTORY_HPP
#define INVENTORY_HPP

#include "dynamic_slot_array.hpp"
#include "inventory_listener.hpp"
#include "item.hpp"
#include "item_schema.hpp"
#include "item_set.hpp"
#include "slot.hpp"
#include "steam_inventory_listener.hpp"

// Enum for slot mode for selection/dragging.
enum InventorySlotMode
{
	SLOT_MODE_ENABLE_ALL,
	SLOT_MODE_FREE_DRAG, // Only excluded disabled; inventory to inventory drag.
	SLOT_MODE_RESTRICTED_DRAG // Multiple item or excluded.
};

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
	~Inventory( void );

	// Inventory handler setter.
	void set_listener( InventoryListener* listener );

    // Item slot book handling.
    SlotArray* get_inventory_slots( void );
    DynamicSlotArray* get_excluded_slots( void );
    DynamicSlotArray* get_selected_slots( void );

	// Item query functions.
	Item* find_item( uint64 unique_id );
	unsigned int get_item_count( void ) const;
	Item* get_item( unsigned int index );

    // Item addition/removal.
	bool add_item( Item* item );
	void delete_item( Item* item );
    bool can_place( const Item* item ) const;
	bool place_item( Item* item );
	bool move_item( Item* item, unsigned int index );
    void displace_item( Item* item );
    void delete_items( void );

	// Item slot state management.
	void set_slot_mode( InventorySlotMode slot_mode );
    bool set_selected( Item* item, bool is_selected );
    void clear_selection( void );

private:

    // Item set and schema.
    ItemSet items_;

	// Slot arrays.
	SlotArray inventory_slots_;
	DynamicSlotArray excluded_slots_;
    DynamicSlotArray selected_slots_;

	// Inventory event listener handle.
	InventoryListener* listener_;

};

#endif // INVENTORY_HPP