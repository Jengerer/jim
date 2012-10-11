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
	bool load_items_from_web();

	// Startup testing.
	bool is_latest_version() const;
	void launch_updater() const;

	// Application running functions.
	void run( void );
	void set_think( bool (ItemManager::*thinkFunction)( void ) );
	bool think( void );
	bool loading( void );
	bool running( void );
	bool exiting( void );

	// Steam handling.
	void handle_callback( void );
	void handle_message( uint32 id, void* message, size_t size );
	void handle_protobuf( uint32 id, void* message, size_t size );

	// Input handling.
	void handle_keyboard( void );

	// JUI::Mouse* input handling.
	virtual bool on_mouse_clicked( JUI::Mouse* mouse );
	virtual bool on_mouse_released( JUI::Mouse* mouse );
	virtual bool on_mouse_moved( JUI::Mouse* mouse );

	// Keyboard input handling.
	virtual bool on_key_pressed( int key );
	virtual bool on_key_released( int key );

	// Slot selection handling.
	void on_slot_clicked( SlotView* slot_view, JUI::Mouse* mouse );
	void on_slot_released( SlotView* slot_view );
	void update_buttons();

	// Popup handling.
	void on_popup_clicked( Popup* popup );
	void on_popup_released( Popup* popup );
	void on_popup_key_pressed( Popup* popup );
	void on_popup_key_released( Popup* popup );

	// Updating displays.
	bool update_item_display( void );
	bool update_page_display( void );

private:

	// Layout handling.
	bool create_layout( void );

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
	SlotBookView* inventory_view_;
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