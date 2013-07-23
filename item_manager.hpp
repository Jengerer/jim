#ifndef ITEM_MANAGER_HPP
#define ITEM_MANAGER_HPP

#include "alert.hpp"
#include "button.hpp"
#include "definition_loader.hpp"
#include "dragged_slot_view.hpp"
#include "dynamic_slot_book.hpp"
#include "http_resource_loader.hpp"
#include "inventory.hpp"
#include "ipopup_handler.hpp"
#include "item_display.hpp"
#include "item_schema.hpp"
#include "notice.hpp"
#include "notification_queue.hpp"
#include "notification.hpp"
#include "popup_display.hpp"
#include "steam_item_handler.hpp"
#include "slot_book.hpp"
#include "slot_grid_view.hpp"
#include <jui/application/application.hpp>
#include <jui/layout/horizontal_split_layout.hpp>
#include <jui/layout/vertical_layout.hpp>
#include <containers/vector.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

class ItemManager: public JUI::Application, public IPopupHandler
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

	// Startup testing.
	bool is_latest_version( void ) const;
	void launch_updater( void ) const;

	// Application running functions.
    JUI::Application::ReturnStatus run( void );
	void set_think( bool (ItemManager::*think_function)( void ) );
	bool think( void );
	bool loading( void );
	bool running( void );
	bool exiting( void );

	// Steam handling.
	bool handle_callback( void );
	bool handle_message( uint32 id, void* message, size_t size );
	bool handle_protobuf( uint32 id, void* message, size_t size );

	// Input handling.
	bool handle_keyboard( void );

	// Mouse input handling.
	virtual JUI::IOResult on_mouse_clicked( JUI::Mouse* mouse );
	virtual JUI::IOResult on_mouse_released( JUI::Mouse* mouse );
	virtual JUI::IOResult on_mouse_moved( JUI::Mouse* mouse );

	// Keyboard input handling.
	virtual JUI::IOResult on_key_pressed( int key );
	virtual JUI::IOResult on_key_released( int key );

    // Generic UI handling.
    JUI::IOResult handle_button_released( JUI::Mouse* mouse );

	// Slot selection handling.
	bool on_slot_clicked( const SlotView* view, Item* item, JUI::Mouse* mouse );
	bool on_slot_released( const SlotView* view, Item* item );
	void update_buttons( void );

	// Popup handling.
	bool on_popup_clicked( Popup* popup );
	bool on_popup_released( Popup* popup );
	bool on_popup_key_pressed( Popup* popup );
	bool on_popup_key_released( Popup* popup );

	// Updating displays.
	void update_item_display( void );
	bool update_page_display( void );

private:

    // Item management.
    Item* create_item_from_message( CSOEconItem* item );

private:

	// Application interfaces.
	SteamItemHandler steam_items_;
	HttpResourceLoader* site_loader_;

    // Inventory and item members.
    SlotBook inventory_book_;
	DynamicSlotBook excluded_book_;
	Inventory inventory_;
    ItemSchema schema_;

	// Display layers.
	Container* user_layer_;

	// Page handling members.
	JUI::WrappedText* page_display_;
	Button* prev_button_;
	Button* next_button_;

	// Item selection handling.
	DWORD page_delay_;

	// Running think function.
	bool (ItemManager::*think_function_)( void );

	// Threaded loader for resources.
	DefinitionLoader* definition_loader_;

};

#endif // ITEM_MANAGER_HPP