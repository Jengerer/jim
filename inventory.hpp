#ifndef INVENTORY_HPP
#define INVENTORY_HPP

#include "dynamic_slot_array.hpp"
#include "inventory_listener.hpp"
#include "item.hpp"
#include "item_schema.hpp"
#include "item_set.hpp"
#include "slot.hpp"
#include "steam_inventory_listener.hpp"

// Application attributes.
const JUTIL::ConstantString APPLICATION_TITLE = "Jengerer's Item Manager";
const JUTIL::ConstantString APPLICATION_VERSION = "0.9.9.9.7.9";

/*
 * Class that manages an inventory book and fallback slots.
 *
 * The main invariant that this class aims to preserve is that
 * item positions must be the same as stored on the Steam backend.
 */
class Inventory : public SteamInventoryListener
{

public:

	Inventory( void );
	~Inventory( void );

	// Inventory handler setter.
	void set_listener( InventoryListener* listener );

    // Item slot book handling.
    SlotArray* get_inventory_slots( void );
    DynamicSlotArray* get_excluded_slots( void );

    // Item schema handling.
    ItemSchema* get_schema( void );
	bool on_schema_loaded( void );

	// Item query functions.
	Item* find_item( uint64 unique_id );

    // Item addition/removal.
    bool can_place( const Item* item ) const;
	bool place_item( Item* item );
    void displace_item( Item* item );
    void delete_items( void );

	// Excluded handling.
	bool resolve_excluded( void );

    // Steam inventory listener handling functions.
    virtual bool on_item_created( Item* item );
    virtual bool on_item_deleted( uint64 unique_id );
    virtual bool on_item_updated( uint64 unique_id, uint32 flags );
    virtual bool on_craft_failed( void );
    virtual bool on_inventory_reset( void );
    virtual bool on_inventory_resize( bool is_trial_account, uint32 extra_slots );
    virtual bool on_inventory_loaded( void );

private:

    // Item set and schema.
    ItemSet items_;
    ItemSchema schema_;

	// Slot arrays.
	SlotArray inventory_slots_;
	DynamicSlotArray excluded_slots_;

	// Inventory event listener handle.
	InventoryListener* listener_;

};

#endif // INVENTORY_HPP