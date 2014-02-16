#ifndef ITEM_MANAGER_VIEW_LISTENER_HPP
#define ITEM_MANAGER_VIEW_LISTENER_HPP

/*
 * Interface for passing events from the item manager UI
 * view to the item manager.
 */
class ItemManagerViewListener
{

public:

    virtual bool on_error_acknowledged( void ) = 0;

	// Handle UI button events.
	virtual bool on_craft_items( void ) = 0;
	virtual bool on_delete_item( void ) = 0;

};

#endif // ITEM_MANAGER_VIEW_LISTENER_HPP