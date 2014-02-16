#ifndef STEAM_INVENTORY_MANAGER_HPP
#define STEAM_INVENTORY_MANAGER_HPP

#include "item_schema.hpp"
#include "steam.hpp"
#include "inventory_action_interface.hpp"
#include "steam_inventory_listener.hpp"

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

	// Pending item update functions.
	bool queue_item_update( const Item* item );
	void remove_item_update( const Item* item );
	bool submit_item_updates( void ) const;
	unsigned int get_pending_updates( void ) const;

	// Single item change functions.
	virtual bool move_item( const Item* item, unsigned int index ) const;
	virtual bool update_item( const Item* item ) const;
	virtual bool delete_item( const Item* item ) const;

	// Craft handling functions.
	virtual bool set_craft_size( unsigned int count );
	virtual void set_craft_item( unsigned int index, const Item* item );
	virtual bool craft_items( void );

private:

    // Internal message handling.
    bool handle_callback( uint32 id, void* message );
    bool handle_message( uint32 id, void* message );
    bool handle_protobuf( uint32 id, void* message, unsigned int size );

    // Item object management.
    Item* create_item_from_message( CSOEconItem* item );

private:

	// Crafting item buffer.
	JUTIL::ArrayBuilder<char> craft_buffer_;
	JUTIL::Map<uint64, const Item*> updated_;

    // Steam inventory listener handle.
    SteamInventoryListener* listener_;

};

#endif // STEAM_INVENTORY_MANAGER_HPP