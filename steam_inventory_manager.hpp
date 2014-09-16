#ifndef STEAM_INVENTORY_MANAGER_HPP
#define STEAM_INVENTORY_MANAGER_HPP

#include "item_schema.hpp"
#include "steam.hpp"
#include "inventory_action_interface.hpp"
#include "steam_inventory_listener.hpp"
#include "protobuf/base_gcmessages.pb.h"
#include "protobuf/steammessages.pb.h"
#include "protobuf/gcsdk_gcmessages.pb.h"

class SteamInventoryManager
    : public Steam,
      public InventoryActionInterface
{

public:

	SteamInventoryManager( void );
	virtual ~SteamInventoryManager( void );

    // Setting inventory listener.
    void set_listener( SteamInventoryListener* listener );

    // Generic message handling functions.
    bool handle_callbacks( void );

	// Single item change functions.
	virtual bool move_item( const Item* item, unsigned int index ) const;
	virtual bool delete_item( const Item* item ) const;

	// Item position update functions.
	void clear_item_updates( void );
	bool add_item_update( const Item* item );
	bool push_item_updates( void );

	// Craft handling functions.
	virtual bool set_craft_size( unsigned int count );
	virtual void set_craft_item( unsigned int index, const Item* item );
	virtual bool craft_items( void );

private:

    // Internal message handling.
    bool handle_callback( uint32 id, void* message );
    bool handle_message( uint32 id, void* message );
    bool handle_protobuf( uint32 id, void* message, unsigned int size, uint64 job_id_source );

    // Item object management.
    Item* create_item_from_message( CSOEconItem* item );

private:

	// Steam message buffers.
	JUTIL::ArrayBuilder<char> craft_buffer_;
	CUpdateItems update_msg_;

    // Steam inventory listener handle.
    SteamInventoryListener* listener_;

};

#endif // STEAM_INVENTORY_MANAGER_HPP