#ifndef ITEM_MANAGER_H
#define ITEM_MANAGER_H

#include <jui/application.h>
#include <jui/horizontal_split_layout.h>
#include <jui/vertical_layout.h>

#include "alert.h"
#include "backpack.h"
#include "button.h"
#include "definition_loader.h"
#include "dragged_slot_view.h"
#include "dynamic_slot_book.h"
#include "http_resource_loader.h"
#include "ipopup_handler.h"
#include "item_display.h"
#include "notice.h"
#include "notification_queue.h"
#include "notification.h"
#include "popup_display.h"
#include "steam_item_handler.h"
#include "slot_book.h"
#include "toggle_set.h"

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include <vector>

const unsigned int BUTTON_SPACING = 5;

class ItemManager: public Application, public IPopupHandler
{
public:

	ItemManager( HINSTANCE instance );
	virtual ~ItemManager();

	// Starting up.
	void load_interfaces();
	void close_interfaces();

	// Inventory and definition loading.
	void load_definitions();
	void load_items_from_web();

	// Startup testing.
	bool is_latest_version() const;
	void launch_updater() const;

	// User interface creation.
	void create_layout( void );

	// Application running functions.
	void run();
	void set_think( void (ItemManager::*thinkFunction)( void ) );
	void think( void );
	void loading( void );
	void running( void );
	void exiting( void );

	// Steam handling.
	void handle_callback( void );
	void handle_message( uint32 id, void* message, size_t size );
	void handle_protobuf( uint32 id, void* message, size_t size );

	// Input handling.
	void handle_keyboard( void );

	// Mouse input handling.
	virtual bool on_mouse_clicked( Mouse *mouse );
	virtual bool on_mouse_released( Mouse *mouse );
	virtual bool on_mouse_moved( Mouse *mouse );

	// Keyboard input handling.
	virtual bool on_key_pressed( int key );
	virtual bool on_key_released( int key );

	// Slot selection handling.
	void on_slot_clicked( SlotView* slotView, Mouse* mouse );
	void on_slot_released( SlotView* slotView );
	void update_buttons();

	// Popup handling.
	void on_popup_clicked( Popup* popup );
	void on_popup_released( Popup* popup );
	void on_popup_key_pressed( Popup* popup );
	void on_popup_key_released( Popup* popup );

	// Updating displays.
	void update_item_display( void );
	void update_page_display( void );

private:

	// Application interfaces.
	SteamItemHandler* steam_items_;
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
	WrappedText* page_display_;
	Button* prev_button_;
	Button* next_button_;

	// Item selection handling.
	SlotView* drag_target_;
	DraggedSlotView* dragged_view_;
	DWORD page_delay_;

	// Running think function.
	void (ItemManager::*think_function_)( void );

	// Threaded loader for resources.
	DefinitionLoader* definition_loader_;

	// Top-level fonts.
	IFont* title_font_;
	IFont* page_font_;

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