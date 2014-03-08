#ifndef INVENTORY_LISTENER_HPP
#define INVENTORY_LISTENER_HPP

#include "item.hpp"

/*
 * Listener interface for handling inventory messages.
 */
class InventoryListener
{
	
public:

	// Handle item position update.
	virtual void on_selection_changed( void ) = 0;

};

#endif // INVENTORY_LISTENER_HPP