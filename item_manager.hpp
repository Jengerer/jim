#ifndef ITEM_MANAGER_HPP
#define ITEM_MANAGER_HPP

#include <jui/application/application.hpp>
#include <jui/layout/horizontal_split_layout.hpp>
#include <jui/layout/vertical_layout.hpp>
#include <containers/vector.hpp>
#include "definition_loader.hpp"
#include "http_resource_loader.hpp"
#include "inventory.hpp"
#include "inventory_listener.hpp"
#include "item_manager_view.hpp"
#include "item_manager_view_listener.hpp"
#include "item_schema.hpp"
#include "steam_inventory_manager.hpp"

class ItemManager
    : public JUI::Application,
      public ItemManagerViewListener,
      public InventoryListener
{

public:

	ItemManager( HINSTANCE instance );
	virtual ~ItemManager( void );

	// Starting up.
	virtual JUI::Application::ReturnStatus initialize( void );
	virtual void close_interfaces( void );
    bool create_resources( void );

	// Inventory and definition loading.
	bool start_definition_load( void );

	// Application running functions.
    JUI::Application::ReturnStatus run( void );
	void set_think( bool (ItemManager::*think_function)( void ) );
	bool think( void );
	bool waiting_for_items( void );
	bool loading_schema( void );
	bool running( void );
	bool exiting( void );

    // View listener functions.
    virtual bool on_error_acknowledged( void );

	// Inventory listener functions.
	virtual bool on_inventory_loaded( void );

	// Input handling.
	bool handle_keyboard( void );

	// Mouse input handling.
	virtual JUI::IOResult on_mouse_clicked( JUI::Mouse* mouse );
	virtual JUI::IOResult on_mouse_released( JUI::Mouse* mouse );
	virtual JUI::IOResult on_mouse_moved( JUI::Mouse* mouse );

	// Keyboard input handling.
	virtual JUI::IOResult on_key_pressed( int key );
	virtual JUI::IOResult on_key_released( int key );

private:

	// Application interfaces.
	HttpResourceLoader* site_loader_;
    DefinitionLoader* definition_loader_;

    // Inventory and item members.
	Inventory inventory_;
    SteamInventoryManager steam_items_;

    // User interface handle.
    ItemManagerView* view_;

	// Running think function.
	bool (ItemManager::*think_function_)( void );

};

#endif // ITEM_MANAGER_HPP