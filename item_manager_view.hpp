#ifndef ITEM_MANAGER_VIEW_HPP
#define ITEM_MANAGER_VIEW_HPP

#include "button_listener.hpp"
#include "button_manager.hpp"
#include "dragged_slot_view.hpp"
#include "inventory.hpp"
#include "item_display.hpp"
#include "item_manager_view_listener.hpp"
#include <jui/io/mouse_handler_interface.hpp>
#include "notification_queue.hpp"
#include "popup_display.hpp"
#include "popup_listener.hpp"
#include "resource_loader_interface.hpp"
#include "slot_book_view.hpp"

/*
 * Class for managing the item manager UI.
 */
class ItemManagerView:
    public JUI::Container,
    public JUI::MouseHandlerInterface,
    public ButtonListener,
    public PopupListener
{

public:

    ItemManagerView( Inventory* inventory );
    virtual ~ItemManagerView( void );

    // Listener pointer setting.
    void set_listener( ItemManagerViewListener* listener );

    // Interface initialization.
    bool download_resources( ResourceLoaderInterface* loader );
    bool initialize( JUI::Graphics2D* graphics );
    void clean_up( void );

    // Top-down UI management functions.
    bool create_alert( const JUTIL::String* message );
    bool create_error( const JUTIL::String* message );
    bool set_loading_notice( const JUTIL::String* message );
    void destroy_loading_notice( void );
    NotificationQueue* get_notification_queue( void );

    // Frame updating.
    bool on_enter_frame( void );

    // Mouse handling functions.
    virtual JUI::IOResult on_mouse_moved( JUI::Mouse* mouse );
    virtual JUI::IOResult on_mouse_clicked( JUI::Mouse* mouse );
    virtual JUI::IOResult on_mouse_released( JUI::Mouse* mouse );

    // Button handling functions.
    virtual bool on_button_pressed( Button* button );
    virtual bool on_button_released( Button* button );

    // Popup listener functions.
    virtual bool on_popup_killed( Popup* popup );

private:

    // Interface sub-initializers.
    bool create_layers( void );
    bool create_layout( JUI::Graphics2D* graphics );

    // UI updating helpers.
    bool update_item_display( void );
    bool update_page_display( void );

private:

    // Inventory to create view for.
    Inventory* inventory_;

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
	Alert* error_;
	bool update_error_;
	Notice* load_progress_;
	Confirmation* craft_check_;
	Confirmation* delete_check_;

    // User interface members.
    ButtonManager button_manager_;
	SlotBookView* inventory_view_;
	SlotBookView* excluded_view_;
    DraggedSlotView* dragged_view_;

    // UI buttons.
	Button* craft_button_;
	Button* sort_button_;
	Button* delete_button_;
    Button* prev_button_;
    Button* next_button_;

    // Top-level fonts.
	JUI::FontInterface* title_font_;
	JUI::FontInterface* page_font_;

    // Item manager event handling interface.
    ItemManagerViewListener* listener_;

};

#endif // ITEM_MANAGER_VIEW_HPP