#ifndef ITEM_MANAGER_VIEW_HPP
#define ITEM_MANAGER_VIEW_HPP

#include "button_listener.hpp"
#include "button_manager.hpp"
#include "inventory.hpp"
#include "item_display.hpp"
#include "item_manager_view_listener.hpp"
#include <jui/io/mouse_handler_interface.hpp>
#include <jui/io/keyboard_handler_interface.hpp>
#include "notification_queue.hpp"
#include "popup_display.hpp"
#include "popup_listener.hpp"
#include "resource_loader_interface.hpp"
#include "slot_book_view.hpp"
#include "slot_stack_view.hpp"
#include "inventory_action_interface.hpp"

// Inventory selection mode.
enum InventorySelectionMode
{
	SELECT_MODE_NORMAL,
	SELECT_MODE_MULTIPLE
};

/*
 * Class for managing the item manager UI.
 */
class ItemManagerView:
    public JUI::Container,
    public JUI::MouseHandlerInterface,
	public JUI::KeyboardHandlerInterface,
    public ButtonListener,
    public PopupListener,
    public SlotArrayViewListener
{

public:

    ItemManagerView( Inventory* inventory, ItemSchema* schema );
    virtual ~ItemManagerView( void );

    // Listener pointer setting.
    void set_listener( ItemManagerViewListener* listener );

    // Interface initialization.
    bool download_resources( ResourceLoaderInterface* loader );
    bool initialize( void );
	bool create_layout( JUI::Graphics2D* graphics );

    // Top-down UI management functions.
    bool create_alert( const JUTIL::String* message );
    bool create_error( const JUTIL::String* message );
    bool set_loading_notice( const JUTIL::String* message );
    void destroy_loading_notice( void );
    NotificationQueue* get_notification_queue( void );

    // Frame updating.
    bool on_enter_frame( void );

	// Page management.
	bool next_page( void );
	bool previous_page( void );
	void excluded_next_page( void );
	void excluded_previous_page( void );
	bool update_page_display( void );
	void update_excluded_page_display( void );

    // Mouse handling functions.
    virtual JUI::IOResult on_mouse_moved( JUI::Mouse* mouse );
    virtual JUI::IOResult on_mouse_clicked( JUI::Mouse* mouse );
    virtual JUI::IOResult on_mouse_released( JUI::Mouse* mouse );

	// Keyboard handling functions.
	virtual JUI::IOResult on_key_pressed( int key );
	virtual JUI::IOResult on_key_released( int key );

    // Button handling functions.
	void update_buttons_state( void );
    virtual bool on_button_pressed( Button* button );
    virtual bool on_button_released( Button* button );

    // Popup listener functions.
    virtual bool on_popup_killed( Popup* popup );

    // Slot array view listener handling.
    virtual bool on_slot_hovered( SlotArrayInterface* slot_array, unsigned int index );
    virtual bool on_slot_clicked( JUI::Mouse* mouse,
		SlotArrayInterface* slot_array,
		unsigned int index );
    virtual bool on_slot_released( SlotArrayInterface* slot_array, unsigned int index );

private:

    // Interface sub-initializers.
    bool create_layers( void );

    // UI updating helpers.
    bool update_item_display( void );

private:

    // Inventory to create view for.
	ItemSchema* schema_;
    Inventory* inventory_;
	InventoryActionInterface* action_interface_;

    // Text elements.
    JUI::WrappedText* page_display_;

    // Display layers.
    JUI::Container* user_layer_;
    PopupDisplay* popups_;

    // Display for item information.
	ItemDisplay* item_display_;

	// Information notifications.
	NotificationQueue* notifications_;

	// Popups.
	Alert* alert_;
	Alert* update_alert_;
	Alert* error_;
	bool update_error_;
	Notice* load_progress_;
	Confirmation* craft_check_;
	Confirmation* delete_check_;

    // User interface members.
    ButtonManager button_manager_;
	SlotBookView* inventory_view_;
	SlotBookView* excluded_view_;

	// Item dragging members.
    SlotStackView* selected_view_;
	const SlotView* clicked_view_;
	bool was_clicked_selected_;
	bool was_dragging_;
	int clicked_x_;
	int clicked_y_;
	long switch_page_time_;

    // UI buttons.
	Button* craft_button_;
	Button* sort_button_;
	Button* delete_button_;
    Button* prev_button_;
    Button* next_button_;
	Button* excluded_prev_button_;
	Button* excluded_next_button_;

    // Top-level fonts.
	JUI::FontInterface* heading_font_;
	JUI::FontInterface* page_font_;

    // Item manager event handling interface.
    ItemManagerViewListener* listener_;

	// Keyboard state members.
	bool is_mouse_down_;
	bool is_multiselect_pressed_;

};

#endif // ITEM_MANAGER_VIEW_HPP