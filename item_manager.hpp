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
#include "item_update_queue.hpp"
#include "steam_inventory_manager.hpp"
#include "updater.hpp"

class ItemManager
    : public JUI::Application,
      public ItemManagerViewListener,
      public InventoryListener,
	  public SteamInventoryListener
{

public:

	ItemManager( HINSTANCE instance );
	virtual ~ItemManager( void );

	// Starting up.
	virtual JUI::Application::ReturnStatus initialize( void );
	virtual void close_interfaces( void );
	virtual void exit_application( void );
    bool create_resources( void );

	// Inventory and definition loading.
	bool start_definition_load( void );
	bool on_schema_loaded( void );

	// Application running functions.
    JUI::Application::ReturnStatus run( void );
	void set_think( bool (ItemManager::*think_function)( void ) );
	bool think( void );
	bool waiting_for_items( void );
	bool loading_schema( void );
	bool running( void );
	bool updating_items( void );
	bool pending_update( void );
	bool updating( void );
	bool exiting( void );

	// Handle pushing updates to Steam.
	void start_pushing_updates( void );
	void update_pushing_notice( void );

    // View listener functions.
    virtual bool on_error_acknowledged( void );
	virtual bool on_craft_items( void );
	virtual bool on_delete_item( void );

	// Inventory listener functions.
	virtual bool on_item_moved( Item* item );
	virtual void on_selection_changed( void );

	// Steam inventory listener functions.
    virtual bool on_item_created( Item* item );
    virtual void on_item_deleted( uint64 unique_id );
    virtual bool on_item_updated( uint64 unique_id, uint32 flags );
    virtual void on_craft_failed( void );
    virtual void on_inventory_reset( void );
    virtual bool on_inventory_resize( bool is_trial_account, uint32 extra_slots );
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
	Updater* updater_;
	boost::thread* update_thread_;

    // Inventory and item members.
	ItemSchema schema_;
	Inventory inventory_;
    SteamInventoryManager steam_items_;
	ItemUpdateQueue updates_;
	bool pending_deletes_;
	bool pushing_updates_;

    // User interface handle.
    ItemManagerView* view_;
    JUTIL::DynamicString title_;

	// Running think function.
	bool (ItemManager::*think_function_)( void );

};

#endif // ITEM_MANAGER_HPP