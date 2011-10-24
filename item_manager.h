#ifndef ITEM_MANAGER_H
#define ITEM_MANAGER_H

#include "alert.h"
#include "application.h"
#include "backpack.h"
#include "button.h"
#include "definition_loader.h"
#include "dragged_slot_view.h"
#include "horizontal_split_layout.h"
#include "ipopup_handler.h"
#include "item_display.h"
#include "notice.h"
#include "notification_queue.h"
#include "notification.h"
#include "popup_display.h"
#include "steam_item_handler.h"
#include "toggle_set.h"
#include "vertical_layout.h"

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include <vector>

const unsigned int BUTTON_SPACING = 5;

class ItemManager: public Application, public IPopupHandler
{
public:

	ItemManager( void );
	virtual ~ItemManager();

	// Starting up.
	void load_interfaces( HINSTANCE instance );
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
	SteamItemHandler*	steamItems_;
	Backpack*			backpack_;

	// Display layers.
	Container*			user_layer_;

	// User interface members.
	SlotGridPages*		inventoryView_;
	SlotGridView*		excludedView_;
	Button*				craftButton_;
	Button*				equipButton_;
	Button*				sortButton_;

	// Page handling members.
	WrappedText*		pageDisplay_;
	Button*				prevButton_;
	Button*				nextButton_;

	// Item selection handling.
	SlotView*			dragTarget_;
	DraggedSlotView*	draggedView_;
	DWORD				pageDelay_;

	// Running think function.
	void (ItemManager::*thinkFunction_)( void );

	// Threaded loader for resources.
	DefinitionLoader*	definitionLoader_;

	// Top-level fonts.
	Font*				titleFont_;
	Font*				pageFont_;

	// Popup manager.
	PopupDisplay*		popups_;

	// Display for item information.
	ItemDisplay*		itemDisplay_;

	// Information notifications.
	NotificationQueue	*notifications_;

	// Popups.
	Alert				*alert_;
	Alert				*error_;
	bool				updateError_;
	Notice				*loadProgress_;
	Confirmation		*craft_check_;
	Confirmation		*delete_check_;

};

#endif // ITEM_MANAGER_H