#ifndef INVENTORY_ACTION_INTERFACE_HPP
#define INVENTORY_ACTION_INTERFACE_HPP

#include "item.hpp"

/*
 * Interface for pushing item changes to backend.
 */
class InventoryActionInterface
{

public:

	// Single item change functions.
	virtual bool move_item( const Item* item, unsigned int index ) const = 0;
	virtual bool delete_item( const Item* item ) const = 0;
	
	// Craft handling functions.
	virtual bool set_craft_size( unsigned int count ) = 0;
	virtual void set_craft_item( unsigned int index, const Item* item ) = 0;
	virtual bool craft_items( void ) = 0;

};

#endif // INVENTORY_ACTION_INTERFACE_HPP