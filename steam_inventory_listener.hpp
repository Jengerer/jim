#ifndef STEAM_INVENTORY_LISTENER_HPP
#define STEAM_INVENTORY_LISTENER_HPP

#include "item.hpp"

/*
 * Interface for handling events produced by the SteamInventoryManager
 * class.
 */
class SteamInventoryListener
{

public:

    // Item state change messages.
    virtual bool on_item_added( Item* item ) = 0;
    virtual bool on_item_deleted( uint64 unique_id ) = 0;
    virtual bool on_item_updated( uint64 unique_id, uint32 flags ) = 0;

    // Item action messages.
    virtual bool on_craft_failed( void ) = 0;

    // Inventory state changes.
    virtual bool on_inventory_reset( void ) = 0;
    virtual bool on_inventory_resize( bool is_trial_account, uint32 extra_slots ) = 0;
    virtual bool on_inventory_loaded( void ) = 0;

    // Schema definitions.
    virtual bool on_schema_loaded( ItemSchema* schema ) = 0;

};

#endif // STEAM_INVENTORY_LISTENER_HPP