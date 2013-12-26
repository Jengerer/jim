#include "item_manager_view.hpp"
#include <jui/application/error_stack.hpp>
#include <jui/layout/horizontal_split_layout.hpp>

// Application attributes.
const JUTIL::ConstantString APPLICATION_TITLE = "Jengerer's Item Manager";
const JUTIL::ConstantString APPLICATION_VERSION = "0.9.9.9.7.9";

// General application layout.
const unsigned int PADDING = 20;
const unsigned int LAYOUT_SPACING = 10;

// Resources for item manager.
const JUTIL::ConstantString TF2_BUILD_FONT = "fonts/tf2build.ttf";
const JUTIL::ConstantString TF2_SECONDARY_FONT = "fonts/tf2secondary.ttf";
const JUTIL::ConstantString CRAFT_ICON_TEXTURE = "img/manager/gear.png";
const JUTIL::ConstantString SORT_ICON_TEXTURE = "img/manager/sort.png";
const JUTIL::ConstantString DELETE_ICON_TEXTURE = "img/manager/delete.png";
const JUTIL::ConstantString UNKNOWN_ITEM_ICON_TEXTURE = "img/backpack/unknown_item.png";

// Title display.
const JUTIL::ConstantString EXCLUDED_HEADING_TEXT = "New/Excluded Items";
const JUTIL::ConstantString SELECTED_HEADING_TEXT = "Selected Items";
const JUTIL::String* HEADING_FONT_FACE = &TF2_BUILD_FONT;
const unsigned int HEADING_FONT_SIZE = 12;
const bool HEADING_FONT_BOLDED = false;
const JUI::Colour HEADING_COLOUR( 241, 239, 237 );

// Page display.
const JUTIL::String* PAGE_FONT_FACE = &TF2_SECONDARY_FONT;
const unsigned int PAGE_FONT_SIZE = 16;
const bool PAGE_FONT_BOLDED = false;
const JUI::Colour PAGE_LABEL_COLOUR( 201, 79, 57 );
const unsigned int PAGE_LABEL_WIDTH = 50;

// Item display attributes.
const unsigned int ITEM_DISPLAY_SPACING	= 10;
const unsigned int ITEM_DISPLAY_ACTIVE_ALPHA = 220;
const unsigned int ITEM_DISPLAY_ALPHA_STEP = 20;

// Button layout.
const unsigned int BUTTON_SPACING = 5;

// Inventory/excluded view dimensions.
const unsigned int INVENTORY_PAGE_WIDTH = 10;
const unsigned int INVENTORY_PAGE_HEIGHT = 5;
const unsigned int EXCLUDED_PAGE_WIDTH = 5;
const unsigned int EXCLUDED_PAGE_HEIGHT = 1;

// Keys for handling UI.
const int MULTIDRAG_KEY_CODE = VK_SHIFT;
const int MULTISELECT_KEY_CODE = VK_CONTROL;

ItemManagerView::ItemManagerView( Inventory* inventory )
    : inventory_( inventory ),
      inventory_view_( nullptr ),
      excluded_view_( nullptr ),
	  load_progress_( nullptr ),
	  alert_( nullptr ),
	  error_( nullptr ),
	  craft_check_( nullptr ),
	  delete_check_( nullptr ),
	  craft_button_( nullptr ),
	  sort_button_( nullptr ),
	  delete_button_( nullptr ),
	  prev_button_( nullptr ),
	  next_button_( nullptr ),
      heading_font_( nullptr ),
      page_font_( nullptr ),
	  multiselect_pressed_( false )
{
    button_manager_.set_event_listener( this );
}

ItemManagerView::~ItemManagerView( void )
{
}

/*
 * Set the handler for item manager view events.
 */
void ItemManagerView::set_listener( ItemManagerViewListener* listener )
{
    listener_ = listener;
}

/*
 * Download resources from item manager file repository of necessary.
 * Returns true on success, false otherwise.
 */
bool ItemManagerView::download_resources( ResourceLoaderInterface* loader )
{
    // Download font files.
    if (!loader->get_resource( &TF2_BUILD_FONT, &TF2_BUILD_FONT )) {
        return false;
    }
    else if (!loader->get_resource( &TF2_SECONDARY_FONT, &TF2_SECONDARY_FONT )) {
        return false;
    }

    // Download button textures.
    if (!loader->get_resource( &CRAFT_ICON_TEXTURE, &CRAFT_ICON_TEXTURE )) {
        return false;
    }
    else if (!loader->get_resource( &SORT_ICON_TEXTURE, &SORT_ICON_TEXTURE )) {
        return false;
    }
    else if (!loader->get_resource( &DELETE_ICON_TEXTURE, &DELETE_ICON_TEXTURE )) {
        return false;
    }
    
    // Download fallback item icon.
    if (!loader->get_resource( &UNKNOWN_ITEM_ICON_TEXTURE, &UNKNOWN_ITEM_ICON_TEXTURE )) {
        return false;
    }
    return true;
}

/*
 * Initialize item manager view based on inventory.
 */
bool ItemManagerView::initialize( void )
{
    // Load title font.
	heading_font_ = JUI::FontFactory::create_font( HEADING_FONT_FACE, HEADING_FONT_SIZE );
    if (heading_font_ == nullptr) {
        return false;
    }

    // Load page font.
	page_font_ = JUI::FontFactory::create_font( PAGE_FONT_FACE, PAGE_FONT_SIZE );
    if (page_font_ == nullptr) {
        return false;
    }

	// Create layers for UI, then initialize layout.
	if (!create_layers()) {
		return false;
	}
    return true;
}

/*
 * Create full layout for item manager.
 */
bool ItemManagerView::create_layout( JUI::Graphics2D* graphics )
{
    // Stack for logging.
    JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

	// Create layout.
	JUI::VerticalLayout* layout;
	if (!JUTIL::BaseAllocator::allocate( &layout )) {
        stack->log( "Failed to allocate vertical layout." );
		return false;
	}
	new (layout) JUI::VerticalLayout();
	if (!user_layer_->add( layout )) {
		JUTIL::BaseAllocator::destroy( layout );
        stack->log( "Failed to add layout to user layer." );
		return false;
	}

	// Create inventory view.
    SlotArray* book = inventory_->get_inventory_slots();
	if (!JUTIL::BaseAllocator::allocate( &inventory_view_ )) {
        stack->log( "Failed to allocate inventory view." );
		return false;
	}
    new (inventory_view_) SlotBookView( book, INVENTORY_PAGE_WIDTH, INVENTORY_PAGE_HEIGHT );
	if (!layout->add( inventory_view_ )) {
		JUTIL::BaseAllocator::destroy( inventory_view_ );
        stack->log( "Failed to add initialized inventory view to layout." );
		return false;
	}
    if (!inventory_view_->initialize()) {
        stack->log( "Failed to resize inventory view grid." );
        return false;
    }
    inventory_view_->set_listener( this );
	book->set_listener( inventory_view_ );

	// Create button layout.
	JUI::HorizontalSplitLayout* button_layout;
	if (!JUTIL::BaseAllocator::allocate( &button_layout )) {
        stack->log( "Failed to allocate button layout." );
		return false;
	}
	int inventory_width = inventory_view_->get_width();
	new (button_layout) JUI::HorizontalSplitLayout( inventory_width );
	if (!layout->add( button_layout )) {
		JUTIL::BaseAllocator::destroy( button_layout );
        stack->log( "Failed to add button layout to layout." );
		return false;
	}

	// Create inventory button layout.
	JUI::HorizontalLayout* inventory_buttons;
	if (!JUTIL::BaseAllocator::allocate( &inventory_buttons )) {
        stack->log( "Failed to allocate inventory button layout." );
		return false;
	}
	inventory_buttons = new (inventory_buttons) JUI::HorizontalLayout();
	if (!button_layout->set_left( inventory_buttons )) {
		JUTIL::BaseAllocator::destroy( inventory_buttons );
        stack->log( "Failed to add inventory buttons to button layout." );
		return false;
	}

	// Create page button layout.
	JUI::HorizontalLayout* page_display_layout;
	if (!JUTIL::BaseAllocator::allocate( &page_display_layout )) {
        stack->log( "Failed to create page display layout." );
		return false;
	}
	page_display_layout = new (page_display_layout) JUI::HorizontalLayout();
	if (!button_layout->set_right( page_display_layout )) {
		JUTIL::BaseAllocator::destroy( page_display_layout );
        stack->log( "Failed to add page display to button layout." );
		return false;
	}

	// Create inventory buttons.
	JUI::FileTexture* craft_texture;
    JUI::FileTexture* sort_texture;
    JUI::FileTexture* delete_texture;
    JUI::Graphics2D::ReturnStatus status;

    // Load craft texture.
    status = graphics->get_texture( &CRAFT_ICON_TEXTURE, &craft_texture );
    if (status != JUI::Graphics2D::Success) {
        return false;
    }

    // Load sort texture.
    status = graphics->get_texture( &SORT_ICON_TEXTURE, &sort_texture );
    if (status != JUI::Graphics2D::Success) {
        return false;
    }

    // Load delete texture.
    status = graphics->get_texture( &DELETE_ICON_TEXTURE, &delete_texture );
    if (status != JUI::Graphics2D::Success) {
        return false;
    }

	// Create buttons.
    const JUTIL::ConstantString CRAFT_BUTTON_LABEL = "craft";
    const JUTIL::ConstantString SORT_BUTTON_LABEL = "sort";
    const JUTIL::ConstantString DELETE_BUTTON_LABEL = "delete";
    craft_button_ = button_manager_.create( &CRAFT_BUTTON_LABEL, craft_texture );
    if (craft_button_ == nullptr) {
        stack->log( "Failed to create craft button." );
        return false;
    }
	if (!inventory_buttons->add( craft_button_ )) {
		button_manager_.remove( craft_button_ );
		stack->log( "Failed to add craft button to layout." );
		return false;
	}
    sort_button_ = button_manager_.create( &SORT_BUTTON_LABEL, sort_texture );
    if (sort_button_ == nullptr ) {
        stack->log( "Failed to create sort button." );
        return false;
    }
	if (!inventory_buttons->add( sort_button_ )) {
		stack->log( "Failed to add sort button to layout." );
		button_manager_.remove( sort_button_ );
		return false;
	}
    delete_button_ = button_manager_.create( &DELETE_BUTTON_LABEL, delete_texture );
    if (delete_button_ == nullptr) {
        stack->log( "Failed to create delete button." );
        return false;
    }
	if (!inventory_buttons->add( delete_button_ )) {
		stack->log( "Failed to add delete button to layout." );
		button_manager_.remove( delete_button_ );
		return false;
	}

	// Set button states.
	craft_button_->set_enabled( false );
	sort_button_->set_enabled( false );
    delete_button_->set_enabled( false );

	// Create previous page button.
	const JUTIL::ConstantString PREV_PAGE_LABEL = "<";
    const JUTIL::ConstantString NEXT_PAGE_LABEL = ">";
    prev_button_ = button_manager_.create( &PREV_PAGE_LABEL, nullptr );
	if (prev_button_ == nullptr) {
        stack->log( "Failed to create previous page button." );
		return false;
	}
	if (!page_display_layout->add( prev_button_ )) {
		JUTIL::BaseAllocator::destroy( prev_button_ );
        stack->log( "Failed to add previous button to page display layout." );
		return false;
	}

	// Create page display text.
	if (!JUTIL::BaseAllocator::allocate( &page_display_ )) {
        stack->log( "Failed to allocate page display text." );
		return false;
	}
	page_display_ = new (page_display_) JUI::WrappedText( page_font_, PAGE_LABEL_WIDTH );
	page_display_->set_colour( &PAGE_LABEL_COLOUR );
	page_display_->set_text_formatting( DT_CENTER );
	if (!page_display_layout->add( page_display_ )) {
		JUTIL::BaseAllocator::destroy( page_display_ );
        stack->log( "Failed to add page display text to page display layout." );
		return false;
	}

    // Create next page button.
    next_button_ = button_manager_.create( &NEXT_PAGE_LABEL, nullptr );
	if (next_button_ == nullptr) {
        stack->log( "Failed to create next page button." );
		return false;
	}
	if (!page_display_layout->add( next_button_ )) {
		JUTIL::BaseAllocator::destroy( next_button_ );
        stack->log( "Failed to add next button to page display layout." );
		return false;
	}

    // Page display created, initialize.
    if (!update_page_display()) {
        stack->log( "Failed to update page display to default." );
        return false;
    }

	// Pack buttons.
	inventory_buttons->pack( BUTTON_SPACING, JUI::ALIGN_TOP );
	page_display_layout->pack( BUTTON_SPACING, JUI::ALIGN_MIDDLE );
	button_layout->pack();

	// Create split layout for headings and excluded/selected slots.
	JUI::HorizontalSplitLayout* heading_split;
	JUI::HorizontalSplitLayout* slot_split;
	if (!JUTIL::BaseAllocator::allocate( &heading_split )) {
		stack->log( "Failed to allocate heading split layout." );
		return false;
	}
	if (!layout->add( heading_split )) {
		JUTIL::BaseAllocator::release( heading_split );
		stack->log( "Failed to add heading split layout. ");
		return false;
	}
	new (heading_split) JUI::HorizontalSplitLayout( inventory_width );
	if (!JUTIL::BaseAllocator::allocate( &slot_split )) {
		stack->log( "Failed to allocate slot split layout." );
		return false;
	}
	if (!layout->add( slot_split )) {
		JUTIL::BaseAllocator::release( slot_split );
		stack->log( "Failed to add slot split layout." );
		return false;
	}
	new (slot_split) JUI::HorizontalSplitLayout( inventory_width );
		
	// Create excluded and multi-drag title.
	JUI::Text* excluded_heading;
	JUI::Text* selected_heading;
	if (!JUTIL::BaseAllocator::allocate( &excluded_heading )) {
		stack->log( "Failed to allocate excluded heading text." );
		return false;
	}
	if (!heading_split->set_left( excluded_heading )) {
		JUTIL::BaseAllocator::release( excluded_heading );
		stack->log( "Failed to add excluded heading to layout." );
		return false;
	}
	new (excluded_heading) JUI::Text( heading_font_ );
	if (!JUTIL::BaseAllocator::allocate( &selected_heading )) {
		stack->log( "Failed to allocate selected heading text." );
		return false;
	}
	if (!heading_split->set_right( selected_heading )) {
		JUTIL::BaseAllocator::release( selected_heading );
		stack->log( "Failed to add selected heading to layout." );
		return false;
	}
	new (selected_heading) JUI::Text( heading_font_ );
	excluded_heading->set_text( &EXCLUDED_HEADING_TEXT );
	selected_heading->set_text( &SELECTED_HEADING_TEXT );
	heading_split->pack();

	// Create dummy selected items view.
	SlotView* dummy_view;
	if (!JUTIL::BaseAllocator::allocate( &dummy_view )) {
		stack->log( "Failed to allocate dummy selected slot." );
		return false;
	}
	if (!slot_split->set_right( dummy_view )) {
		JUTIL::BaseAllocator::release( &dummy_view );
		stack->log( "Failed to add dummy selected slot to layout." );
		return false;
	}
	new (dummy_view) SlotView();
	if (!dummy_view->initialize()) {
		stack->log( "Failed to initialize dummy slot view." );
		return false;
	}

	// Now put actual selected slot view on top of dummy.
    book = inventory_->get_selected_slots();
    if (!JUTIL::BaseAllocator::allocate( &selected_view_ )) {
        stack->log( "Failed to allocate selected item view." );
        return false;
    }
    if (!add( selected_view_ )) {
        JUTIL::BaseAllocator::release( &selected_view_ );
        stack->log( "Failed to add selected item view to user layer." );
        return false;
    }
	new (selected_view_) SlotStackView( book );
    if (!selected_view_->initialize()) {
        stack->log( "Failed to initialize selected item view." );
        return false;
    }
    book->set_listener( selected_view_ );
	selected_view_->set_listener( this );
	
	// Create excluded and selected view.
    book = inventory_->get_excluded_slots();
	if (!JUTIL::BaseAllocator::allocate( &excluded_view_ )) {
        stack->log( "Failed to allocate excluded slot view." );
		return false;
	}
	if (!slot_split->set_left( excluded_view_ )) {
		JUTIL::BaseAllocator::release( excluded_view_ );
        stack->log( "Failed to add initialized excluded view to layout." );
		return false;
	}
	new (excluded_view_) SlotBookView( book, EXCLUDED_PAGE_WIDTH, EXCLUDED_PAGE_HEIGHT );
	if (!excluded_view_->initialize()) {
		stack->log( "Failed to initialize excluded slot view." );
		return false;
	}
    excluded_view_->set_listener( this );
	book->set_listener( excluded_view_ );
	slot_split->pack();

	// Pack top-most layout.
	layout->pack( LAYOUT_SPACING, JUI::ALIGN_LEFT );
	int center_x = (get_width() - layout->get_width()) / 2;
	int center_y = (get_height() - layout->get_height()) / 2;
	layout->set_position( center_x, center_y );

	// Move dragged slot view over dummy.
	int dummy_x = dummy_view->get_x();
	int dummy_y = dummy_view->get_y();
	selected_view_->set_position( dummy_x, dummy_y );

	// Create item display.
    ItemSchema* schema = inventory_->get_schema();
	if (!JUTIL::BaseAllocator::allocate( &item_display_ )) {
        stack->log( "Failed to allocate item display." );
		return false;
	}
	item_display_ = new (item_display_) ItemDisplay( schema );
	if (!item_display_->initialize()) {
		JUTIL::BaseAllocator::destroy( item_display_ );
		stack->log( "Failed to initialize item display." );
		return false;
	}
	else if (!user_layer_->add( item_display_ )) {
		JUTIL::BaseAllocator::destroy( item_display_ );
        stack->log( "Failed to add item display to user layer." );
		return false;
	}

	// All created successfully.
	return true;
}

/*
 * Clean up item manager view objects.
 * This should be called before font factory destroyed.
 */
void ItemManagerView::clean_up( void )
{
    // Remove font resources.
    if (heading_font_ != nullptr) {
        JUI::FontFactory::destroy_font( heading_font_ );
    }
    if (page_font_ != nullptr) {
        JUI::FontFactory::destroy_font( page_font_ );
    }
}

/*
 * Create generic alert to display.
 * There should be at most one alert at a time.
 */
bool ItemManagerView::create_alert( const JUTIL::String* message )
{
    Alert* alert = popups_->create_alert( message );
    if (alert == nullptr) {
        return false;
    }
    alert_ = alert;
    return true;
}

/*
 * Create and display an error popup to the user.
 */
bool ItemManagerView::create_error( const JUTIL::String* message )
{
    Alert* error = popups_->create_alert( message );
    if (error == nullptr) {
        return false;
    }
    error_ = error;
    return true;
}

/*
 * Set message to be displayed in the loading popup notice.
 */
bool ItemManagerView::set_loading_notice( const JUTIL::String* message )
{
    // Create loading notice if none exists.
    if (load_progress_ == nullptr) {
        load_progress_ = popups_->create_notice( message );
        if (load_progress_ == nullptr) {
            return false;
        }
    }
    else {
        load_progress_->set_message( message );
		load_progress_->pack();
		load_progress_->center_to( popups_ );
    }
    return true;
}

/*
 * Destroy the loading notice from the user on load complete/error.
 */
void ItemManagerView::destroy_loading_notice( void )
{
    assert( load_progress_ != nullptr );
    popups_->delete_popup( load_progress_ );
}

/*
 * Get a handle to the notification queue.
 */
NotificationQueue* ItemManagerView::get_notification_queue( void )
{
    return notifications_;
}

/*
 * Handle per-frame events.
 */
bool ItemManagerView::on_enter_frame( void )
{
	if (!update_item_display()) {
		return false;
	}
	return true;
}

/*
 * Update page display with the current displayed page number.
 */
bool ItemManagerView::update_page_display( void )
{
    // Update page display to show current page.
    unsigned int active_page = inventory_view_->get_active_page() + 1;
    unsigned int total_pages = inventory_view_->get_page_count();
    JUTIL::DynamicString string;
    if (!string.write( "%u/%u", active_page, total_pages )) {
        return false;
    }
    page_display_->set_text( &string );
    return true;
}

/*
 * Handle mouse movement event.
 */
JUI::IOResult ItemManagerView::on_mouse_moved( JUI::Mouse* mouse )
{
    // Pass to popup handler.
    JUI::IOResult result = popups_->on_mouse_moved( mouse );
    if (result != JUI::IO_RESULT_UNHANDLED) {
        return result;
    }

	// Item dragging.
	result = selected_view_->on_mouse_moved( mouse );
	if (result != JUI::IO_RESULT_UNHANDLED) {
		return result;
	}

    // Handle item containers.
	item_display_->set_item( nullptr );
    if (inventory_view_ != nullptr) {
        result = inventory_view_->on_mouse_moved( mouse );
        if (result != JUI::IO_RESULT_UNHANDLED) {
            return result;
        }
    }
	if (excluded_view_ != nullptr) {
        result = excluded_view_->on_mouse_moved( mouse );
        if (result != JUI::IO_RESULT_UNHANDLED) {
            return result;
        }
    }
    
	// Pass to button manager.
	result = button_manager_.on_mouse_moved( mouse );
	if (result != JUI::IO_RESULT_UNHANDLED) {
		return result;
	}
    return result;
}

/*
 * Handle mouse click event.
 */
JUI::IOResult ItemManagerView::on_mouse_clicked( JUI::Mouse* mouse )
{
    // Pass to popup handler.
    JUI::IOResult result = popups_->on_mouse_clicked( mouse );
    if (result != JUI::IO_RESULT_UNHANDLED) {
        return result;
    }

    // Handle item containers.
    result = inventory_view_->on_mouse_clicked( mouse );
    if (result != JUI::IO_RESULT_UNHANDLED) {
        return result;
    }
	result = excluded_view_->on_mouse_clicked( mouse );
    if (result != JUI::IO_RESULT_UNHANDLED) {
        return result;
    }
	result = selected_view_->on_mouse_clicked( mouse );
	if (result != JUI::IO_RESULT_UNHANDLED) {
		return result;
	}

	// Pass to button manager.
	result = button_manager_.on_mouse_clicked( mouse );
	if (result != JUI::IO_RESULT_UNHANDLED) {
		return result;
	}

	// No items or buttons clicked, deselect all if not in multi-mode.
	if (!multiselect_pressed_) {
		inventory_->clear_selection();
	}
    return JUI::IO_RESULT_UNHANDLED;
}

/*
 * Handle mouse release event.
 */
JUI::IOResult ItemManagerView::on_mouse_released( JUI::Mouse* mouse )
{
    // Pass to popup handler.
	JUI::IOResult result = popups_->on_mouse_released( mouse );
    if (result != JUI::IO_RESULT_UNHANDLED) {
        return result;
    }

	// Release item if holding any.
	result = selected_view_->on_mouse_released( mouse );
	if (result == JUI::IO_RESULT_HANDLED) {
		// Handle inventory release only if we're dragging something.
		result = inventory_view_->on_mouse_released( mouse );
		inventory_->set_slot_mode( SLOT_MODE_ENABLE_ALL );
		if (result != JUI::IO_RESULT_UNHANDLED) {
			return result;
		}
		return JUI::IO_RESULT_HANDLED;
	}

	// Pass to button manager.
	result = button_manager_.on_mouse_released( mouse );
	if (result != JUI::IO_RESULT_UNHANDLED) {
		return result;
	}
    return JUI::IO_RESULT_UNHANDLED;
}

/*
 * Handle key press events.
 */
JUI::IOResult ItemManagerView::on_key_pressed( int key )
{
	if (key == MULTISELECT_KEY_CODE) {
		multiselect_pressed_ = true;
	}
	else {
		return JUI::IO_RESULT_UNHANDLED;
	}
	return JUI::IO_RESULT_HANDLED;
}

/*
 * Handle key release events.
 */
JUI::IOResult ItemManagerView::on_key_released( int key )
{
	if (key == MULTISELECT_KEY_CODE) {
		multiselect_pressed_ = false;
	}
	else {
		return JUI::IO_RESULT_UNHANDLED;
	}
	return JUI::IO_RESULT_HANDLED;
}

/*
 * Handle button press events.
 */
bool ItemManagerView::on_button_pressed( Button* button )
{
    return true;
}

/*
 * Handle button release events.
 */
bool ItemManagerView::on_button_released( Button* button )
{
	// Page view variables.
	unsigned int active;
	unsigned int new_active;
    if (button == craft_button_) {
    }
    else if (button == sort_button_) {
    }
    else if (button == delete_button_) {
    }
	// Handle page navigation events.
    else if (button == prev_button_) {
		active = inventory_view_->get_active_page();
		if (active != 0) {
			new_active = active - 1;
			inventory_view_->set_active_page( new_active );
		}
		if (!update_page_display()) {
			return false;
		}
    }
    else if (button == next_button_) {
		active = inventory_view_->get_active_page();
		new_active = active + 1;
		if (new_active < inventory_view_->get_page_count()) {
			inventory_view_->set_active_page( new_active );
		}
		if (!update_page_display()) {
			return false;
		}
    }
    return true;
}

/*
 * Handle UI popup events for confirmations and alerts.
 */
bool ItemManagerView::on_popup_killed( Popup* popup )
{
    // Tell parent that we closed an error if it was killed.
    if (popup == error_) {
        if (!listener_->on_error_acknowledged()) {
            return false;
        }
    }
    return true;
}

/*
 * Handle slot hover event.
 */
bool ItemManagerView::on_slot_hovered( SlotArrayInterface* slot_array, unsigned int index )
{
    // Check which container is triggering this.
    const SlotView* view;
    const Slot* slot = slot_array->get_slot( index );
	const SlotArray* inventory = inventory_->get_inventory_slots();
	const SlotArray* excluded = inventory_->get_excluded_slots();
	const SlotArray* selected = inventory_->get_selected_slots();
    if (slot_array == inventory) {
        view = inventory_view_->get_slot_view( index );
    }
    else if (slot_array == excluded) {
        view = excluded_view_->get_slot_view( index );
    }
	else if (slot_array == selected) {
		view = selected_view_;
	}
    if (!item_display_->set_item( slot->get_item() )) {
		return false;
	}

	// Check if we can place the display below slot view.
	unsigned int x = view->get_x() + (view->get_width() - item_display_->get_width()) / 2;
	unsigned int y = view->get_y() + view->get_height() + ITEM_DISPLAY_SPACING;
	unsigned int bottom = y + item_display_->get_height();
	if (bottom > get_height() - ITEM_DISPLAY_SPACING) {
		y = view->get_y() - item_display_->get_height() - ITEM_DISPLAY_SPACING;
	}
	item_display_->set_position( x, y );
	clamp_child( item_display_, ITEM_DISPLAY_SPACING );
    return true;
}

/*
 * Handle slot click event.
 */
bool ItemManagerView::on_slot_clicked( JUI::Mouse* mouse,
	SlotArrayInterface* slot_array,
	unsigned int index )
{
	// Ignore empty slots.
	const Slot* slot = slot_array->get_slot( index );
	if (!slot->has_item()) {
		return true;
	}
	const SlotView* view;
    SlotArray* container;
    SlotArray* inventory = inventory_->get_inventory_slots();
    SlotArray* excluded = inventory_->get_excluded_slots();
	SlotArray* selected = inventory_->get_selected_slots();

	// Disable item display and full slots.
	item_display_->set_active( false );

    // Check which container we're clicking from.
	bool disable_item_display = false;
	bool restrict_drag = false;
	if (slot_array == selected) {
		container = selected;
		view = selected_view_;
		restrict_drag = true;
		disable_item_display = true;
	}
    else {
		if (slot_array == inventory) {
			container = inventory;
			view = inventory_view_->get_slot_view( index );
		}
		else if (slot_array == excluded) {
			container = excluded;
			view = excluded_view_->get_slot_view( index );
		}

		// Handle single item selection if no modifier key.
		if (multiselect_pressed_) {
			bool new_selected = !slot->is_selected();
			if (!container->set_selected( index, new_selected )) {
				return false;
			}
			if (!inventory_->set_selected( slot, new_selected )) {
				return false;
			}
		}
		else {
			// Hide item display.
			disable_item_display = true;

			// Set this as the new selected item.
			inventory_->clear_selection();
			if (!container->set_selected( index, true )) {
				return false;
			}
			if (!inventory_->set_selected( slot, true )) {
				return false;
			}

			// Disable full slots if excluded.
			restrict_drag = (container == excluded);

			// Start dragging.
			int offset_x = view->get_x() - mouse->get_x();
			int offset_y = view->get_y() - mouse->get_y();
			selected_view_->set_drag_offset( offset_x, offset_y );		
			selected_view_->begin_dragging();
		}
	}

	// Hide item display if indicated.
	if (disable_item_display) {
		item_display_->set_active( false );
	}

	// Set slot mode to restricted if indicated.
	if (restrict_drag) {
		inventory_->set_slot_mode( SLOT_MODE_RESTRICTED_DRAG );
	}
    return true;
}

/*
 * Handle slot release event.
 */
bool ItemManagerView::on_slot_released( SlotArrayInterface* slot_array, unsigned int index )
{
    const Slot* slot = slot_array->get_slot( index );
    SlotArray* inventory = inventory_->get_inventory_slots();
    SlotArray* excluded = inventory_->get_excluded_slots();
	SlotArray* selected = inventory_->get_selected_slots();

	// Don't allow dropping on disabled.
	if (!slot->is_enabled()) {
		return false;
	}

	// If we're dropping onto a full slot, this must be one item, inventory to inventory.
	if (slot->has_item() && !slot->is_selected()) {
		unsigned int dragged_index;
		const Slot* dragged_slot = selected->get_slot( 0 );
		Item* replaced_item = slot->get_item();
		Item* dragged_item = dragged_slot->get_item();
		inventory->contains_item( dragged_item, &dragged_index );
		inventory_->clear_selection();
		inventory->set_item( index, dragged_item );
		inventory->set_item( dragged_index, replaced_item );
	}
	else {
		unsigned int i;
		unsigned int j;
		unsigned int page_size = inventory_view_->get_grid_size();
		unsigned int last_checked = index;
		unsigned int end = inventory->get_size();
		unsigned int selected_end = selected->get_size();

		// Fit all selected on this page.
		for (j = 0; j < selected_end; ++j) {
			Item* current = selected->get_item( j );
			for (i = last_checked; i < end; ++i) {
				const Slot* slot = inventory->get_slot( i );
				if (slot->is_selected() || !slot->has_item()) {
					inventory_->displace_item( current );
					inventory->set_item( i, current );
					inventory->set_selected( i, true );
					last_checked = i + 1;
					break;
				}
			}
		}
	}
    return true;
}

/*
 * Create the user and popup layer and add them to the interface.
 */
bool ItemManagerView::create_layers( void )
{
    // Error stack for logging.
    JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

	// Create the user layer.
	if (!JUTIL::BaseAllocator::allocate( &user_layer_ ))
	{
        stack->log( "Failed to allocate user layer." );
		return false;
	}
	user_layer_ = new (user_layer_) Container();
	user_layer_->set_size( get_width(), get_height() );
	if (!add( user_layer_ ))
	{
		JUTIL::BaseAllocator::destroy( user_layer_ );
        stack->log( "Failed to add user layer to application." );
		return false;
	}

	// Create popup layer on top.
	if (!JUTIL::BaseAllocator::allocate( &popups_ )) {
        stack->log( "Failed to allocate popup layer." );
		return false;
	}
	new (popups_) PopupDisplay();
	if (!add( popups_ )) {
		JUTIL::BaseAllocator::destroy( popups_ );
        stack->log( "Failed to add popup layer to application." );
		return false;
	}
	popups_->set_size( get_width(), get_height() );
	popups_->set_popup_listener( this );
	return true;
}

/*
 * Update the visual effect of the item display.
 */
bool ItemManagerView::update_item_display( void )
{
	item_display_->update_alpha();
    return true;
}