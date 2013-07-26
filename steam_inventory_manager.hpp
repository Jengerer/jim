#ifndef STEAM_INVENTORY_MANAGER_HPP
#define STEAM_INVENTORY_MANAGER_HPP

#include "steam.hpp"
#include "steam_inventory_interface.hpp"

class SteamInventoryManager : public Steam, public SteamInventoryInterface
{

public:

	SteamInventoryManager( void );
	virtual ~SteamInventoryManager( void );

	// Single item change functions.
	virtual bool update_item( const Item* item ) const;
	virtual bool delete_item( const Item* item ) const;

	// Craft handling functions.
	virtual bool set_craft_size( unsigned int count );
	virtual void set_craft_item( unsigned int index, const Item* item );
	virtual bool craft_items( void );

private:

	// Crafting item buffer.
	JUTIL::ArrayBuilder<char> craft_buffer_;

};

#endif // STEAM_INVENTORY_MANAGER_HPP