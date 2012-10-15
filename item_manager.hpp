#ifndef ITEM_MANAGER_H
#define ITEM_MANAGER_H

#include <jui/application/application.hpp>
#include <jui/layout/horizontal_split_layout.hpp>
#include <jui/layout/vertical_layout.hpp>

#include "alert.hpp"
#include "backpack.hpp"
#include "button.hpp"
#include "definition_loader.hpp"
#include "dragged_slot_view.hpp"
#include "dynamic_slot_book.hpp"
#include "http_resource_loader.hpp"
#include "ipopup_handler.hpp"
#include "item_display.hpp"
#include "notice.hpp"
#include "notification_queue.hpp"
#include "notification.hpp"
#include "popup_display.hpp"
#include "steam_item_handler.hpp"
#include "slot_book.hpp"

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include <containers/vector.hpp>

const unsigned int BUTTON_SPACING = 5;

class ItemManager: public JUI::Application, public IPopupHandler
{
public:

	ItemManager( HINSTANCE instance );
	virtual ~ItemManager();

	// Starting up.
	virtual JUI::Application::ReturnStatus initialize( void );
	virtual void close_interfaces( void );
    bool create_resources( void );

	// Inventory and definition loading.
	bool start_definition_load();

	// Startup testing.
	bool is_latest_version() const;
	void launch_updater() const;

	// Application running functions.
    JUI::Application::ReturnStatus run( void );
	void set_think( bool (ItemManager::*thinkFunction)( void ) );
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
	bool on_slot_clicked( SlotView* slot_view, JUI::Mouse* mouse );
	bool on_slot_released( SlotView* slot_view );
	void update_buttons();

	// Popup handling.
	bool on_popup_clicked( Popup* popup );
	bool on_popup_released( Popup* popup );
	bool on_popup_key_pressed( Popup* popup );
	bool on_popup_key_released( Popup* popup );

	// Updating displays.
	void update_item_display( void );
	bool update_page_display( void );

private:

	// Layout handling.
	bool create_layout( void );
	bool create_layers( void );

    // Item management.
    Item* create_item_from_message( CSOEconItem* item );

private:

	// Application interfaces.
	SteamItemHandler steam_items_;
	SlotBook* inventory_book_;
	DynamicSlotBook* excluded_book_;
	Backpack* backpack_;
	HttpResourceLoader* site_loader_;

	// Display layers.
	Container* user_layer_;

	// User interface members.
	AnimatedBookView* inventory_view_;
	SlotBookView* excluded_view_;
	Button* craft_button_;
	Button* equip_button_;
	Button* sort_button_;
	Button* delete_button_;

	// Page handling members.
	JUI::WrappedText* page_display_;
	Button* prev_button_;
	Button* next_button_;

	// Item selection handling.
	DraggedSlotView* dragged_view_;
	DWORD page_delay_;

	// Running think function.
	bool (ItemManager::*think_function_)( void );

	// Threaded loader for resources.
	DefinitionLoader* definition_loader_;

	// Top-level fonts.
	JUI::FontInterface* title_font_;
	JUI::FontInterface* page_font_;

	// Popup manager.
	PopupDisplay* popups_;

	// Display for item information.
	ItemDisplay* item_display_;

	// Information notifications.
	NotificationQueue* notifications_;

	// Popups.
	Alert* alert_;
	Alert* error_;
	bool update_error_;
	Notice* load_progress_;
	Confirmation* craft_check_;
	Confirmation* delete_check_;

};

#endif // ITEM_MANAGER_H