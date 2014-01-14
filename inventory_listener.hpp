#ifndef INVENTORY_LISTENER_HPP
#define INVENTORY_LISTENER_HPP

#include "item.hpp"

/*
 * Listener interface for handling inventory messages.
 */
class InventoryListener
{
	
public:

	// One time trigger for inventory load event.
	virtual bool on_inventory_loaded( void ) = 0;

	// Handle item position update.
	virtual bool on_item_moved( Item* item ) = 0;

};

#endif // INVENTORY_LISTENER_HPP