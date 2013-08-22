#include "item_manager_view.hpp"
#include <jui/application/error_stack.hpp>
#include <jui/layout/horizontal_split_layout.hpp>

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
const JUTIL::String* TITLE_FONT_FACE = &TF2_BUILD_FONT;
const unsigned int TITLE_FONT_SIZE = 14;
const bool TITLE_FONT_BOLDED = false;
const JUI::Colour TITLE_COLOUR( 241, 239, 237 );

// Page display.
const JUTIL::String* PAGE_FONT_FACE = &TF2_SECONDARY_FONT;
const unsigned int PAGE_FONT_SIZE = 16;
const bool PAGE_FONT_BOLDED = false;
const JUI::Colour PAGE_LABEL_COLOUR( 201, 79, 57 );
const unsigned int PAGE_LABEL_WIDTH = 50;

// Item display attributes.
const unsigned int ITEM_DISPLAY_SPACING	= 10;

// Button layout.
const unsigned int BUTTON_SPACING = 5;

ItemManagerView::ItemManagerView( Inventory* inventory )
    : inventory_( inventory ),
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
      title_font_( nullptr ),
      page_font_( nullptr )
{
    button_manager_.set_event_listener( this );
}

ItemManagerView::~ItemManagerView( void )
{
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
bool ItemManagerView::initialize( JUI::Graphics2D* graphics )
{
    // Load title font.
	title_font_ = JUI::FontFactory::create_font( TITLE_FONT_FACE, TITLE_FONT_SIZE );
    if (title_font_ == nullptr) {
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
	if (!create_layout( graphics )) {
		return false;
	}
    return true;
}

/*
 * Clean up item manager view objects.
 * This should be called before font factory destroyed.
 */
void ItemManagerView::clean_up( void )
{
    // Remove font resources.
    if (title_font_ != nullptr) {
        JUI::FontFactory::destroy_font( title_font_ );
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
    if (button == craft_button_) {
    }
    else if (button == sort_button_) {
    }
    else if (button == delete_button_) {
    }
    else if (button == prev_button_) {
    }
    else if (button == next_button_) {
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

	// Create title.
	JUTIL::DynamicString title;
	if (!title.write( "%s %s", APPLICATION_TITLE.get_string(), APPLICATION_VERSION.get_string() )) {
        stack->log( "Failed to write title string." );
		return false;
	}
	JUI::Text* title_text;
	if (!JUTIL::BaseAllocator::allocate( &title_text )) {
		stack->log( "Failed to allocate title text element." );
		return false;
	}
	new (title_text) JUI::Text( title_font_ );
	title_text->set_text( &title );
	title_text->set_colour( &TITLE_COLOUR );
	if (!layout->add( title_text )) {
		JUTIL::BaseAllocator::destroy( title_text );
        stack->log( "Failed to add title to layout." );
		return false;
	}

	// Create inventory view.
    SlotBook* book = inventory_->get_inventory_book();
    unsigned int page_width = book->get_page_width();
    unsigned int page_height = book->get_page_height();
	if (!JUTIL::BaseAllocator::allocate( &inventory_view_ )) {
        stack->log( "Failed to allocate inventory view." );
		return false;
	}
    new (inventory_view_) SlotGridView();
	if (!layout->add( inventory_view_ )) {
		JUTIL::BaseAllocator::destroy( inventory_view_ );
        stack->log( "Failed to add initialized inventory view to layout." );
		return false;
	}
    if (!inventory_view_->set_grid_size( page_width, page_height )) {
        stack->log( "Failed to resize inventory view grid." );
        return false;
    }

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
	page_display_layout->pack( BUTTON_SPACING, JUI::ALIGN_TOP );
	button_layout->pack();

	// Create excluded view.
    book = inventory_->get_excluded_book();
    page_width = book->get_page_width();
    page_height = book->get_page_height();
	if (!JUTIL::BaseAllocator::allocate( &excluded_view_ )) {
        stack->log( "Failed to allocate excluded slot view." );
		return false;
	}
	excluded_view_ = new (excluded_view_) SlotGridView();
	if (!excluded_view_->set_grid_size( page_width, page_height ) || !layout->add( excluded_view_ )) {
		JUTIL::BaseAllocator::destroy( excluded_view_ );
        stack->log( "Failed to add initialized excluded view to layout." );
		return false;
	}

	// Pack top-most layout.
	layout->pack( LAYOUT_SPACING, JUI::ALIGN_LEFT );
	int center_x = (get_width() - layout->get_width()) / 2;
	int center_y = (get_height() - layout->get_height()) / 2;
	layout->set_position( center_x, center_y );

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
 * Handle UI popup events for confirmations and alerts.
 */
bool ItemManagerView::on_popup_killed( Popup* popup )
{
    return true;
}

/*
 * Update the item display with the current item we're hovering over.
 */
bool ItemManagerView::update_item_display( void )
{
    return true;
}

/*
 * Update page display with the current displayed page number.
 */
bool ItemManagerView::update_page_display( void )
{
    return true;
}