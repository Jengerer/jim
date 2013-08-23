#ifndef INVENTORY_LISTENER_HPP
#define INVENTORY_LISTENER_HPP

/*
 * Listener interface for handling inventory messages.
 */
class InventoryListener
{
	
public:

	virtual bool on_inventory_loaded( void ) = 0;

};

#endif // INVENTORY_LISTENER_HPP