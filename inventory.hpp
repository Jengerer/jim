#ifndef INVENTORY_HPP
#define INVENTORY_HPP

#include "dynamic_slot_book.hpp"
#include "item.hpp"
#include "item_schema.hpp"
#include "item_set.hpp"
#include "slot.hpp"
#include "slot_book.hpp"
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

    // Item slot book handling.
    SlotBook* get_inventory_book( void );
    DynamicSlotBook* get_excluded_book( void );

    // Item schema handling.
    ItemSchema* get_schema( void );
    bool resolve_definitions( void );

	// Item query functions.
	Item* find_item( uint64 unique_id ) const;
    Item* get_inventory_item( unsigned int index ) const;
    Item* get_excluded_item( unsigned int index ) const;

    // Item addition/removal.
    bool can_place( const Item* item ) const;
	bool place_item( Item* item, unsigned int index );
    void displace_item( Item* item );
    void remove_item( Item* item );
	void remove_items( void );
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
    virtual bool on_schema_loaded( ItemSchema* schema );

private:

	// Slot arrays.
	SlotBook inventory_book_;
	DynamicSlotBook excluded_book_;
    ItemSet items_;

};

#endif // INVENTORY_HPP