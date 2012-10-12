#include <algorithm>
#include <stdlib.h>
#include <crtdbg.h>
#include <fstream>
#include <math.h>
#include <windows.h>
#include <shellapi.h>
#include <json/json.h>

#include <jui/gfx/font_factory.hpp>
#include <jui/application/error_stack.hpp>

#include "serialized_buffer.hpp"
#include "slot_view.hpp"

#include "protobuf/base_gcmessages.pb.h"
#include "protobuf/steammessages.pb.h"
#include "protobuf/gcsdk_gcmessages.pb.h"

#include "http_resource_loader.hpp"
#include "item_manager.hpp"

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#endif

// Temporarily disable equip and sort.
#define EQUIP_NOT_IMPLEMENTED
#define SORT_NOT_IMPLEMENTED

// Application attributes.
const JUTIL::ConstantString APPLICATION_TITLE = "Jengerer's Item Manager Lite";
const JUTIL::ConstantString APPLICATION_VERSION = "0.9.9.9.7.9";
const int APPLICATION_WIDTH	= 795;
const int APPLICATION_HEIGHT = 540;

// Updater resources.
const JUTIL::ConstantString UPDATER_PATH = "auto_updater.exe";

// UI attributes.
const unsigned int EXIT_BUTTON_PADDING	= 10;
const DWORD PAGE_DELAY_INTERVAL			= 500;

// Item manager resource loading URL.
const JUTIL::ConstantString MANAGER_ROOT_URL = "http://www.jengerer.com/item_manager";

// Resources for item manager.
const JUTIL::ConstantString TF2_BUILD_FONT = "fonts/tf2build.ttf";
const JUTIL::ConstantString TF2_SECONDARY_FONT = "fonts/tf2secondary.ttf";
const JUTIL::ConstantString ROUNDED_CORNER_TEXTURE = "img/manager/rounded_corner.png";
const JUTIL::ConstantString CRAFT_ICON_TEXTURE = "img/manager/gear.png";
const JUTIL::ConstantString EQUIP_ICON_TEXTURE = "img/manager/equip.png";
const JUTIL::ConstantString SORT_ICON_TEXTURE = "img/manager/sort.png";
const JUTIL::ConstantString UNKNOWN_ITEM_ICON_TEXTURE = "img/backpack/unknown_item.png";

// Title display.
const JUTIL::String* TITLE_FONT_FACE = &TF2_BUILD_FONT;
const unsigned int TITLE_FONT_SIZE = 14;
const bool TITLE_FONT_BOLDED = false;
const JUI::Colour TITLE_COLOUR( 241, 239, 237 );

// Page display.
const JUTIL::String* PAGE_FONT_FACE = &TF2_SECONDARY_FONT;
const unsigned int PAGE_FONT_SIZE = 14;
const bool PAGE_FONT_BOLDED				= false;
const JUI::Colour PAGE_LABEL_COLOUR( 201, 79, 57 );
const unsigned int PAGE_LABEL_WIDTH		= 50;

// Item display attributes.
const unsigned int ITEM_DISPLAY_SPACING	= 10;

// Frame rate limiters.
const DWORD FRAME_SPEED = 1000 / 60;

// Inventory attributes.
const int PAGE_WIDTH = 10;
const int PAGE_HEIGHT = 5;
const int PAGE_COUNT = 6;
const int DEFAULT_PAGE_COUNT = 1;
const int TRIAL_PAGE_COUNT = 1;
const int EXCLUDED_WIDTH = 5;
const int EXCLUDED_HEIGHT = 1;

// Slot layout.
const int SLOT_SPACING		= 5;
const int PAGE_SPACING		= 50;

const unsigned int PADDING	= 20;
const unsigned int SPACING	= 10;

/*
 * Item manager constructor.
 */
ItemManager::ItemManager( HINSTANCE instance ) : Application( instance )
{
	// Set application size.
	set_size( APPLICATION_WIDTH, APPLICATION_HEIGHT );

	// Set window parameters.
	JUI::Window* window = get_window();
	window->set_title( APPLICATION_TITLE );
	window->set_border( true );
	window->set_fullscreen( false );	

	// Alerts and errors.
	alert_ = nullptr;
	error_ = nullptr;
	update_error_ = false;

    // Inventory variables.
    backpack_ = nullptr;
    inventory_book_ = nullptr;
    excluded_book_ = nullptr;

	// Item stats.
	item_display_ = nullptr;

	// Slot views.
	inventory_view_ = nullptr;
	excluded_view_ = nullptr;

	// Dragged slot.
	dragged_view_ = nullptr;

	// Threaded loader.
	definition_loader_ = nullptr;
	load_progress_ = nullptr;

	// Fonts.
	page_font_ = nullptr;
	title_font_ = nullptr;

	// Create Steam interface.
	page_delay_ = 0;

	// Set default running function.
	set_think( &ItemManager::loading );
}

/*
 * Item manager destructor.
 */
ItemManager::~ItemManager( void )
{
	close_interfaces();
}

/*
 * Loads basic resources for item manager.
 */
JUI::Application::ReturnStatus ItemManager::initialize( void )
{
    // Prepare stack for logging.
    JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

	// Start up graphics and window.
	JUI::Application::ReturnStatus status = Application::initialize();
    if (status != Success) {
        return status;
    }

	// Get downloader.
	JUI::FileDownloader* downloader = JUI::FileDownloader::get_instance();
	site_loader_ = new HttpResourceLoader( &MANAGER_ROOT_URL, downloader );

	// Download rounded corner.
	site_loader_->get_resource( &ROUNDED_CORNER_TEXTURE, &ROUNDED_CORNER_TEXTURE );

	// Download button icons.
	site_loader_->get_resource( &EQUIP_ICON_TEXTURE, &EQUIP_ICON_TEXTURE );
	site_loader_->get_resource( &CRAFT_ICON_TEXTURE, &CRAFT_ICON_TEXTURE );
	site_loader_->get_resource( &SORT_ICON_TEXTURE, &SORT_ICON_TEXTURE );

	// Get placeholder item icon.
	site_loader_->get_resource( &UNKNOWN_ITEM_ICON_TEXTURE, &UNKNOWN_ITEM_ICON_TEXTURE );

	// Download fonts.
	site_loader_->get_resource( &TF2_BUILD_FONT, &TF2_BUILD_FONT );
	site_loader_->get_resource( &TF2_SECONDARY_FONT, &TF2_SECONDARY_FONT );

	// Necessary to display progress/status.
	if (!Notice::precache( &graphics_ )) {
        return PrecacheResourcesFailure;
    }
	else if (!Button::precache( &graphics_ )) {
        return PrecacheResourcesFailure;
    }
	else if (!RoundedRectangle::precache( &graphics_ )) {
        return PrecacheResourcesFailure;
    }

    // Base resources successful; load extra resources.
    if (!create_resources()) {
        // Check for error to display.
        const JUTIL::String* top = stack->get_top_error();
        error_ = popups_->create_alert( top );
        if (error_ == nullptr) {
            return PrecacheResourcesFailure;
        }
    }

    // All base resources loaded successfully.
    return Success;
}

bool ItemManager::create_resources( void )
{
    // Enough resources loaded to show styled messages.
    JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();
    if (!ItemDisplay::precache()) {
        return false;
    }
	else if (!Notification::precache()) {
        return false;
    }
    else if (!SlotView::precache( &graphics_ )) {
        return false;
    }

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

	// Create notification queue.
    if (!JUTIL::BaseAllocator::allocate( &notifications_ )) {
        stack->log( "Failed to create notification queue!" );
        return false;
    }
	notifications_ = new (notifications_) NotificationQueue();
	notifications_->set_position(
		static_cast<float>(get_width() - PADDING), 
		static_cast<float>(get_height() - PADDING) );
	if (!add( notifications_ )) {
        stack->log( "Failed to add notification queue!" );
        return false;
    }

    // Create inventory.
    if (!JUTIL::BaseAllocator::allocate( &inventory_book_ )) {
        stack->log( "Failed to create inventory book!" );
        return false;
    }
    inventory_book_ = new (inventory_book_) SlotBook( PAGE_WIDTH, PAGE_HEIGHT );
    if (!inventory_book_->add_pages( DEFAULT_PAGE_COUNT )) {
        stack->log( "Failed to populate inventory book!" );
        return false;
    }

    // Create excluded slots.
    if (!JUTIL::BaseAllocator::allocate( &excluded_book_ )) {
        stack->log( "Failed to create excluded item book!" );
        return false;
    }
    excluded_book_ = new (excluded_book_) DynamicSlotBook( EXCLUDED_WIDTH, EXCLUDED_HEIGHT );
    if (!excluded_book_->initialize()) {
        stack->log( "Failed to populate excluded item book!" );
        return false;
    }

	// Create backpack.
    if (!JUTIL::BaseAllocator::allocate( &backpack_ )) {
        stack->log( "Failed to create backpack!" );
        return false;
    }
	backpack_ = new (backpack_) Backpack( inventory_book_, excluded_book_ );

	// Generate UI.
	if (!create_layout()) {
        return false;
    }

	// Check for latest version.
	if (!is_latest_version()) {
        stack->log( "A new version of the item manager is out and needs to be downloaded against your will. Press okay to continue." );
		update_error_ = true;
        return false;
	}

	// Ensure TF2's not running.
	if (get_process_count( "tf2.exe" ) > 0) {
		stack->log( "Please close Team Fortress 2 before running the item manager." );
        return false;
	}
	else if (get_process_count( "item_manager.exe" ) > 1) {
        stack->log( "Another instance of the item manager is already running!" );
        return false;
	}

	// Show progress notice.
    const JUTIL::ConstantString PREPARING_MESSAGE = "Preparing to load item definitions...";
	load_progress_ = popups_->create_notice( &PREPARING_MESSAGE );
    if (load_progress_ == nullptr) {
        stack->log( "Failed to create loading notice." );
        return false;
    }

    // Start loading item definitions.
	if (!start_definition_load()) {
        return false;
    }

    return true;
}

void ItemManager::close_interfaces( void )
{
    // Remove font resources.
    JUI::FontFactory::destroy_font( title_font_ );
    JUI::FontFactory::destroy_font( page_font_ );

    // Delete backpack resources.
    JUTIL::BaseAllocator::safe_destroy( &backpack_ );
    JUTIL::BaseAllocator::safe_destroy( &inventory_book_ );
    JUTIL::BaseAllocator::safe_destroy( &excluded_book_ );

    // Delete definition resources.
    JUTIL::BaseAllocator::safe_destroy( &definition_loader_ );
    JUTIL::BaseAllocator::safe_destroy( &Item::fallback );

	// Erase item definitions.
    InformationMap::Iterator i;
	for (i = Item::definitions.begin(); i.has_next(); i.next()) {
		delete i.get_value();
	}
    Item::definitions.clear();

	// Erase attribute definitions.
    AttributeMap::Iterator j;
    for (j = Item::attributes.begin(); j.has_next(); j.next()) {
        JUTIL::BaseAllocator::destroy( j.get_value() );
	}
    Item::attributes.clear();

	// Delete site loader.
    JUTIL::BaseAllocator::safe_destroy( &site_loader_ );

	// Free cached resources.
	ItemDisplay::release();
	Button::release();
	Notice::release();
	Notification::release();
	SlotView::release();

	// Close font library.
	JUI::FontFactory::shut_down();

	// Free all protobuf library resources.
	google::protobuf::ShutdownProtobufLibrary();

	// Close downloader.
	JUI::FileDownloader::shut_down();
}

void ItemManager::run( void )
{
    Application::run();
	
    // Attempt to think; print error if failed.
    const JUTIL::ConstantString DEFAULT_ERROR = "An unknown error has occurred!";
    if (!think()) {
        // Create error message.
        JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();
        const JUTIL::String* error = stack->get_top_error();
        if (error == nullptr) {
            error = &DEFAULT_ERROR;
        }
        error_ = popups_->create_alert( error );
        
        // Fallback if couldn't display error.
        if (error_ == nullptr) {
            MessageBox( nullptr, "An error has occurred!", error->get_string(), MB_ICONERROR | MB_OK );
        }
        set_think( &ItemManager::exiting );
    }
	draw_frame();
}

void ItemManager::set_think( bool (ItemManager::*think_function)( void ) )
{
	think_function_ = think_function;
}

bool ItemManager::think( void )
{
	return (this->*think_function_)();
}

bool ItemManager::loading( void )
{
	if (definition_loader_ != nullptr) {
		switch (definition_loader_->get_state()) {
		case LOADING_STATE_ERROR:
        {
			// Put error on stack.
            JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();
            stack->log( "%s", definition_loader_->get_progress_msg()->get_string() );

			// Remove threaded loader.
            JUTIL::BaseAllocator::safe_destroy( &definition_loader_ );
            return false;
        }

		case LOADING_STATE_FINISHED:
        {
			// Remove threaded loader.
            JUTIL::BaseAllocator::safe_destroy( &definition_loader_ );
			
			// Initialize steam.
            if (!steam_items_.load_interfaces()) {
                JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();
                stack->log( "Failed to initialize Steam." );
                return false;
			}

            const JUTIL::ConstantString WAITING_MESSAGE = "Waiting for Steam message...";
			load_progress_->set_message( &WAITING_MESSAGE );
			load_progress_->center_to( this );
			break;
        }

		default:
			definition_loader_->update_progress_msg();
			load_progress_->set_message( definition_loader_->get_progress_msg() );
			load_progress_->center_to( this );
			break;
		}
	}
	else {
		handle_callback();
		if (backpack_ != nullptr && backpack_->is_loaded()) {
			set_think( &ItemManager::running );
			popups_->remove_popup( load_progress_ );
#ifndef SORT_NOT_IMPLEMENTED
			sort_button_->set_enabled( true );
#endif
		}
	}
}

bool ItemManager::running( void )
{
	handle_callback();
	notifications_->update_notifications();
	inventory_view_->update_view();
	update_item_display();
    return true;
}

bool ItemManager::exiting( void )
{
	// Just wait for exit.
    return true;
}

bool ItemManager::on_mouse_clicked( JUI::Mouse* mouse )
{
	// JUI::Mouse* clicked.
	if (popups_->on_mouse_clicked( mouse )) {
		return true;
	}
	else {
		if (dragged_view_ != nullptr) {
			return true;
		}

		// Find slot view if touched.
		SlotView* slot_view = nullptr;
		if (mouse->is_touching( inventory_view_ )) {
			slot_view = inventory_view_->get_touching_slot( mouse );
		}
		else if (mouse->is_touching( excluded_view_ )) {
			slot_view = excluded_view_->get_touching_slot( mouse );
		}

		if (slot_view != nullptr) {
			on_slot_clicked( slot_view, mouse );
			return true;
		}
		else {
			// TODO: Set a 'clicked target' pointer so we can't just release on button and trigger.
			if (mouse->is_touching( craft_button_ ) ||
				mouse->is_touching( equip_button_ ) ||
				mouse->is_touching( sort_button_ ) ||
				mouse->is_touching( next_button_ ) ||
				mouse->is_touching( prev_button_ )) {
					return true;
			}
			else if (notifications_->on_mouse_clicked( mouse )) {
				return true;
			}
		}

		// Deselect all, nothing clicked.
		if (steam_items_.get_select_mode() != SELECT_MODE_MULTIPLE) {
			steam_items_.deselect_all();
			update_buttons();
		}
	}

	return false;
}

bool ItemManager::on_mouse_released( JUI::Mouse* mouse )
{
	// Check top popup.
	if (popups_->on_mouse_released( mouse )) {
		return true;
	}
	else {
		// Release slot if dragging.
		if (dragged_view_ != nullptr) {
			SlotView* slot_view = inventory_view_->get_touching_slot( mouse );
			on_slot_released( slot_view );
		}
		else {
			// Handle buttons.
			if (craft_button_->on_mouse_released( mouse )) {
				if (steam_items_.is_selected_tradable()) {
					steam_items_.craft_selected();
				}
				else {
                    const JUTIL::ConstantString TRADABLE_WARNING = "One or more of the items you've selected are not tradable. The result will not be tradable. Continue?";
					craft_check_ = popups_->create_confirmation( &TRADABLE_WARNING );
				}
			}
			else if (next_button_->on_mouse_released( mouse )) {
				inventory_view_->next_page();
				update_page_display();
			}
			else if (prev_button_->on_mouse_released( mouse )) {
				inventory_view_->previous_page();
				update_page_display();
			}
		}
	}

	return false;
}

bool ItemManager::on_mouse_moved( JUI::Mouse* mouse )
{
	// Update buttons.
	// TODO: Have a button frame mouse hover state.
	if (item_display_ != nullptr) {
		item_display_->set_item( nullptr );
	}

	// Pass to highest popup.
	if (popups_->on_mouse_moved( mouse )) {
		return true;
	}
	else {
		// Check if dragging.
		if (dragged_view_ != nullptr) {
			dragged_view_->on_mouse_moved( mouse );
			clamp_child( dragged_view_ );

			// Check if we're switching page.
			DWORD currentTick = GetTickCount();
			if (currentTick > page_delay_) {
				if (dragged_view_->get_x() <= 0.0f) {
					if (inventory_view_->previous_page()) {
						page_delay_ = currentTick + PAGE_DELAY_INTERVAL;
					}

					update_page_display();
				}
				else if (dragged_view_->get_x() + dragged_view_->get_width() >= get_width()) {
					if (inventory_view_->next_page()) {
						page_delay_ = currentTick + PAGE_DELAY_INTERVAL;
					}

					update_page_display();
				}
			}	
		}
		else {
			// Check if we've hovering over an slot view.
			SlotView* slotView = nullptr;
			if (mouse->is_touching( inventory_view_ )) {
				slotView = inventory_view_->get_touching_slot( mouse );
			}
			else if (mouse->is_touching( excluded_view_ )) {
				slotView = excluded_view_->get_touching_slot( mouse );
			}
			else {
				return false;
			}
			
			// Did we get one?
			if (slotView != nullptr) {
				Slot* slot = slotView->get_slot();
				// Only display for full slots.
				if (slot->has_item()) {
					item_display_->set_item( slot->get_item() );
					float displayX = slotView->get_x() + (slotView->get_width() - item_display_->get_width()) / 2.0f;
					float displayY = slotView->get_y() + slotView->get_height() + ITEM_DISPLAY_SPACING;
					if (displayY + item_display_->get_height() > get_height()) {
						displayY = slotView->get_y() - item_display_->get_height() - ITEM_DISPLAY_SPACING;
					}

					item_display_->set_position(
						static_cast<float>(displayX),
						static_cast<float>(displayY) );
					clamp_child( item_display_, static_cast<float>(PADDING) );
				}
			}
		}

		return true;
	}

	return false;
}

void ItemManager::on_slot_clicked( SlotView* slot_view, JUI::Mouse* mouse )
{
	Slot* slot = slot_view->get_slot();
	if (slot->has_item()) {
		switch (steam_items_.get_select_mode()) {
		case SELECT_MODE_SINGLE:
			{
				// Don't allow movement into full slots if excluded.
				Item* item = slot->get_item();
				if (backpack_->is_excluded( item )) {
					inventory_view_->disable_full();
				}
				else {
					// Just disable self.
					slot_view->set_enabled( false );
				}

				// Excluded are never allowed as targets.
				excluded_view_->set_enabled( false );
				
				assert( dragged_view_ == nullptr );
				steam_items_.deselect_all();

				// Start dragging.
				float view_x = slot_view->get_x();
				float view_y = slot_view->get_y();
				dragged_view_ = new DraggedSlotView( slot );
				dragged_view_->set_position( view_x, view_y );
				dragged_view_->set_offset( view_x - mouse->get_x(), view_y - mouse->get_y() );
				dragged_view_->set_alpha( 200 );
				steam_items_.select( dragged_view_ );
				add( dragged_view_ );
				break;
			}

		case SELECT_MODE_MULTIPLE:
			steam_items_.toggle_select( slot_view );
			break;
		}
	}
	else if (steam_items_.get_select_mode() != SELECT_MODE_MULTIPLE) {
		steam_items_.deselect_all();
	}

	update_buttons();
}

void ItemManager::on_slot_released( SlotView* slot_view )
{
	// Remove item from backpack temporarily.
	Slot* dragged_slot = dragged_view_->get_slot();
	Item* dragged_item = dragged_slot->get_item();

	// Reset selections.
	inventory_view_->set_enabled( true );
	excluded_view_->set_enabled( true );

	// Check if item came from excluded.
	bool is_excluded = backpack_->is_excluded( dragged_item );

	// Delete temporary dragged.
	remove( dragged_view_ );
	delete dragged_view_;
	dragged_view_ = nullptr;

	// Do we have a slot to place it in?
	if (slot_view != nullptr) {
		Slot* touched_slot = slot_view->get_slot();
		if (touched_slot->has_item()) {
			// Swap slots if not excluded.
			if (!is_excluded) {
				Item* touched_item = touched_slot->get_item();
				unsigned int old_index = dragged_item->get_index();
				unsigned int new_index = touched_item->get_index();
				backpack_->move_item( dragged_item, new_index );
				backpack_->move_item( touched_item, old_index );
				steam_items_.update_item( dragged_item );
				steam_items_.update_item( touched_item );
			}
		}
		else {
			dragged_slot->remove_item();
			backpack_->move_item( dragged_item, touched_slot->get_index() );
			steam_items_.update_item( dragged_item );
		}
	}

	update_buttons();
}

void ItemManager::update_buttons()
{
	unsigned int selectedCount = steam_items_.get_selected_count();
	craft_button_->set_enabled( selectedCount != 0 );

#ifndef EQUIP_NOT_IMPLEMENTED
	equip_button_->set_enabled( steam_items_.can_equip_selected() );
#endif
}

/*
 * Handle keyboard pressed input.
 */
bool ItemManager::on_key_pressed( int key )
{
	// Now handle ourselves.
	switch (key) {
	case VK_ESCAPE:
		exit_application();
		break;

	case VK_RETURN:
		popups_->on_key_pressed( key );
		break;

	case VK_CONTROL:
		steam_items_.set_select_mode( SELECT_MODE_MULTIPLE );
		break;

	case VK_LEFT:
		inventory_view_->previous_page();
		update_page_display();
		break;

	case VK_RIGHT:
		inventory_view_->next_page();
		update_page_display();
		break;
	}

	return true;
}

/*
 * Handle keyboard release input.
 */
bool ItemManager::on_key_released( int key )
{
	// Now handle ourselves.
	switch (key) {
	case VK_CONTROL:
		steam_items_.set_select_mode( SELECT_MODE_SINGLE );
		break;
	}

	return true;
}

void ItemManager::update_item_display( void )
{
	item_display_->update_alpha();
}

/*
 * Update display for page numbers.
 */
bool ItemManager::update_page_display( void )
{
	// Update text.
    JUTIL::DynamicString page_string;
    unsigned int active_page = inventory_view_->get_active_page() + 1;
    unsigned int total_pages = inventory_view_->get_page_count();
    if (!page_string.write( "%u/%u", active_page, total_pages )) {
        return false;
    }
	page_display_->set_text( &page_string );

	// Update buttons.
	unsigned int page_index = inventory_view_->get_active_page();
	unsigned int last_index = inventory_view_->get_page_count() - 1;
	prev_button_->set_enabled( page_index != 0 );
	next_button_->set_enabled( page_index != last_index );
    return true;
}

bool ItemManager::start_definition_load( void )
{
	// Set the message and redraw.
    const JUTIL::ConstantString LOADING_DEFINITION_MESSAGE = "Loading item definitions...";
	load_progress_->set_message( &LOADING_DEFINITION_MESSAGE );

	// Set up loader.
    // TODO: Don't need to allocate this; can use the "done" flag in loader instead of checking pointer null.
    if (!JUTIL::BaseAllocator::allocate( &definition_loader_ )) {
        return false;
    }
    definition_loader_ = new (definition_loader_) DefinitionLoader( &graphics_ );
	if (!definition_loader_->begin()) {
        return false;
    }
    return true;
}

bool ItemManager::is_latest_version( void ) const
{
	// Check for program updates.
	const JUTIL::ConstantString VERSION_URL = "http://www.jengerer.com/item_manager/item_manager.txt";
	JUI::FileDownloader* downloader = JUI::FileDownloader::get_instance();
	if (downloader != nullptr) {
		JUTIL::DynamicString version;
		if (downloader->read( &VERSION_URL, &version )) {
			if (!version.is_equal( &APPLICATION_VERSION )) {
				return false;
			}
		}
	}

	// If failed to get version, assume latest.
	return true;
}

void ItemManager::launch_updater( void ) const
{
	// Get updater if not exists.
	site_loader_->get_resource( &UPDATER_PATH, &UPDATER_PATH );

	// TODO: Make an error type enum and launch this on update error.
	int result = (int)ShellExecute( 0, 0, "auto_updater.exe", 0, 0, SW_SHOWDEFAULT );
	if (result <= 32) {
		MessageBox( nullptr, "Failed to run updater, try re-downloading the application manually.", "Automatic update error!", MB_ICONEXCLAMATION | MB_OK );
	}
}

void ItemManager::handle_callback( void ) {
	CallbackMsg_t callback;
	if ( steam_items_.get_callback( &callback ) ) {
		switch (callback.m_iCallback) {
		case GCMessageAvailable_t::k_iCallback:
			{
				GCMessageAvailable_t *message = (GCMessageAvailable_t *)callback.m_pubParam;
				
				uint32 size;
				if ( steam_items_.has_message( &size ) )
				{
					uint32 id, realSize = 0;
					BYTE* buffer = new BYTE[size];

					try {
						steam_items_.get_message( &id, buffer, size, &realSize );

						// Filter protobuf messages.
						if ((id & 0x80000000) != 0) {
							uint32 realId = id & 0x0FFFFFFF;

							// First get the protobuf struct header.
							SerializedBuffer headerBuffer(buffer);
							GCProtobufHeader_t *headerStruct = headerBuffer.get<GCProtobufHeader_t>();
							uint32 headerSize = headerStruct->m_cubProtobufHeader;

							// Now get the real protobuf header.
							CMsgProtoBufHeader headerMsg;
							void *headerBytes = headerBuffer.here();
							headerMsg.ParseFromArray( headerBytes, headerSize );
							headerBuffer.push( headerSize );

							// Check if we can set target ID.
							// TODO: Maybe move all this horseshit into Steam.
							if ( headerMsg.has_job_id_source() ) {
								steam_items_.set_target_id( headerMsg.job_id_source() );
							}

							uint32 bodySize = size - sizeof( GCProtobufHeader_t ) - headerSize;
							handle_protobuf( realId, headerBuffer.here(), bodySize );
						}
						else {
							handle_message( id, buffer, size );
						}
					}
					catch (const std::bad_alloc& alloc_ex) {
						throw alloc_ex;
					}
					catch (const std::runtime_error& ex) {
						delete buffer;
						throw ex;
					}

					delete buffer;
				}
			}
		}

		steam_items_.release_callback();
	} 
}

void ItemManager::handle_message( uint32 id, void* message, size_t size )
{
	switch (id) {
		case GCCraftResponse_t::k_iMessage:
		{
			GCCraftResponse_t *craftMsg = static_cast<GCCraftResponse_t*>(message);
			if (craftMsg->blueprint == 0xffff) {
				const JUTIL::ConstantString CRAFT_FAIL_MESSAGE = "Crafting failed; no such blueprint!";
				notifications_->add_notification( &CRAFT_FAIL_MESSAGE, nullptr );
			}

			break;
		}
	}
}

void ItemManager::handle_protobuf( uint32 id, void* message, size_t size )
{

#ifdef _DEBUG
	ofstream log;
	log.open( "message_log.txt", ios::app );
	log << "Got message of type " << id << ".\n";
	log.close();
#endif

	switch (id) {
	case k_ESOMsg_CacheSubscribed:
		{
			// Get message.
			CMsgSOCacheSubscribed cache_msg;
			cache_msg.ParseFromArray( message, size );

#ifdef _DEBUG
			ofstream buf;
			buf.open( "buf.txt", ios::binary );
			buf.write( (const char*)message, size );
			buf.close();
#endif

			steam_items_.set_version( cache_msg.version() );

			// Check that this is our backpack.
			if (cache_msg.owner() != steam_items_.get_steam_id()) {
				break;
			}

			// Empty backpack.
			backpack_->remove_items();
								
			// Add items.
			for (int i = 0; i < cache_msg.objects_size(); i++) {
				CMsgSOCacheSubscribed_SubscribedType subscribed_type = cache_msg.objects( i );
				switch (subscribed_type.type_id()) {
				case 1:
					{
						// Ensure we own this item.
						for (int i = 0; i < subscribed_type.object_data_size(); i++) {
							CSOEconItem econ_item;
							econ_item.ParseFromArray( subscribed_type.object_data( i ).data(), 
								subscribed_type.object_data( i ).size() );
							Item *item = new Item(
								econ_item.id(),
								econ_item.def_index(),
								econ_item.level(),
								(EItemQuality)econ_item.quality(),
								econ_item.quantity(),
								econ_item.inventory(),
								econ_item.origin() );

							// Add the item's attributes.
							for (int j = 0; j < econ_item.attribute_size(); ++j) {
								const CSOEconItemAttribute& attribute = econ_item.attribute( j );
								uint32 attrib_index = attribute.def_index();
									
								// Get attribute information.
								auto k = Item::attributes.find( attrib_index );
								if (k != Item::attributes.end()) {
									const AttributeInformation* info = k->second;
										
									// Set value based on type.
									uint32 int_value = attribute.value();
									void* value = (void*)&int_value;
									Attribute* new_attribute;
									if (info->is_integer()) {
										new_attribute = new Attribute( info, *(uint32*)value );
									}
									else {
										new_attribute = new Attribute( info, *(float*)value );
									}

									item->add_attribute( new_attribute );
								}
							}

							// Set custom name.
							if (econ_item.has_custom_name()) {
								item->set_custom_name( econ_item.custom_name() );
							}

							// Finalize state.
							item->update_attributes();
							backpack_->insert_item( item );
						}

						break;
					}
				case 7:
					{
						if (steam_items_.get_steam_id() == cache_msg.owner()) {
							for (int i = 0; i < subscribed_type.object_data_size(); i++) {
								CSOEconGameAccountClient client;
								client.ParseFromArray( subscribed_type.object_data( i ).data(), 
									subscribed_type.object_data( i ).size() );

								// Check how many slots this backpack is supposed to have.
								unsigned int slots = client.additional_backpack_slots();
								if (client.trial_account()) {
									slots += PAGE_WIDTH * PAGE_HEIGHT * TRIAL_PAGE_COUNT;
								}
								else {
									slots += PAGE_WIDTH * PAGE_HEIGHT * PAGE_COUNT;
								}

								// Check that we don't reduce slot count.
								if (slots < inventory_book_->get_slot_count()) {
									throw std::runtime_error( "New backpack size smaller than old." );
								}

								// Add the newer slots.
								unsigned int added_slots = slots - inventory_book_->get_slot_count();
								unsigned int added_pages = added_slots / inventory_book_->get_page_size();
								inventory_book_->add_pages( added_pages );
								inventory_view_->update_pages();
								update_page_display();
							}
						}

						break;
					}
				}
			}

			backpack_->resolve_excluded();
			backpack_->set_loaded( true );

			// Create layout if this is the first cache.
			notifications_->add_notification( "Backpack successfully loaded from Steam.", nullptr );

			break;
		}

	case 27:
		{
			CMsgSOCacheSubscriptionCheck check;
			check.ParseFromArray( message, size );

			// Compare version.
			uint64 version = check.version();
			if (steam_items_.get_version() != version) {
				steam_items_.set_version( version );

				// Send refresh.
				CMsgSOCacheSubscriptionRefresh refresh;
				refresh.set_owner( steam_items_.get_steam_id() );
				string refreshString = refresh.SerializeAsString();
				steam_items_.send_message( 28 | 0x80000000, (void*)refreshString.c_str(), refreshString.size() );
			}
			break;
		}

	case k_ESOMsg_Update:
		{
			CMsgSOSingleObject update_msg;
			update_msg.ParseFromArray( message, size );
			steam_items_.set_version( update_msg.version() );

			if (update_msg.type_id() == 1) {
				CSOEconItem updated_item;
				updated_item.ParseFromArray( update_msg.object_data().data(), update_msg.object_data().size() );

				// Find updated item.
				Item* target = backpack_->find_item( updated_item.id() );
				if (target == nullptr) {
					break;
				}

				// Place item into excluded, to be resolved later.
				backpack_->remove_item( target );
				target->set_flags( updated_item.inventory() );
				backpack_->insert_item( target );
			}

			break;
		}

	case k_ESOMsg_UpdateMultiple:
		{
			CMsgSOMultipleObjects update_msg;
			update_msg.ParseFromArray( message, size );
			steam_items_.set_version( update_msg.version() );

			for (int i = 0; i < update_msg.objects_size(); i++) {
				CMsgSOMultipleObjects::SingleObject current_object = update_msg.objects( i );
				if (current_object.type_id() == 1) {
					CSOEconItem updated_item;
					updated_item.ParseFromArray( current_object.object_data().data(), current_object.object_data().size() );

					// Attempt to find the item.
					Item* target = backpack_->find_item( updated_item.id() );
					if (target != nullptr) {
						// TODO: bump old into excluded.
						backpack_->remove_item( target );
						target->set_flags( updated_item.inventory() );
						backpack_->insert_item( target );
					}
				}
			}

			break;
		}

	case k_EMsgGCUpdateItemSchema:
		{
			// Not handling yet.
			break;
		}

	case SOMsgCreate_t::k_iMessage:
		{
			// Get object created.
			CMsgSOSingleObject create_msg;
			create_msg.ParseFromArray( message, size );
			steam_items_.set_version( create_msg.version() );

			// Get item from object.
			CSOEconItem created_item;
			created_item.ParseFromArray( create_msg.object_data().data(), create_msg.object_data().size() );

			// Now add item.
			Item* new_item = new Item(
				created_item.id(),
				created_item.def_index(),
				created_item.level(),
				(EItemQuality)created_item.quality(),
				created_item.quantity(),
				created_item.inventory(),
				created_item.origin() );

			// Add this item to excluded.
			backpack_->insert_item( new_item );
			
			// Get the source.
			string source;
			if (created_item.origin() == 4) {
				source = "crafted";
			}
			else {
				source = "found";
			}

			// Display message.
			notifications_->add_notification( "You have " + source + " a " + new_item->get_name() + ".", new_item->get_texture() );

			break;
		}

	case SOMsgDeleted_t::k_iMessage:
		{
			// Get deleted message.
			CMsgSOSingleObject delete_msg;
			delete_msg.ParseFromArray( message, size );
			steam_items_.set_version( delete_msg.version() );

			// Get ID of deleted item.
			CSOEconItem deleted_item;
			deleted_item.ParseFromArray( delete_msg.object_data().data(), delete_msg.object_data().size() );

			// Now remove from inventory.
			Item* target = backpack_->find_item( deleted_item.id() );
			if (target != nullptr) {
				backpack_->remove_item( target );

				// Make sure the deleted item isn't being dragged.
				if (dragged_view_ != nullptr) {
					Slot* dragged_slot = dragged_view_->get_slot();
					if (dragged_slot->get_item() == target) {
						// TODO: Dragged dies many ways; make a function?
						remove( dragged_view_ );
						delete dragged_view_;
						dragged_view_ = nullptr;
					}
				}

				delete target;
			}

			break;
		}

	default:
		break;
	}
}

void ItemManager::on_popup_clicked( Popup* popup )
{
	// Nothing.
}

void ItemManager::on_popup_released( Popup* popup )
{
	// Exited popup handling.
	if (popup->is_killed()) {
		if (popup == error_) {
			// Update if that's the reason we're exiting.
			if (update_error_) {
				launch_updater();
			}

			exit_application();
		}
		else if (popup == craft_check_) {
			if (craft_check_->get_response() == RESPONSE_YES) {
				steam_items_.craft_selected();
				update_buttons();
			}
		}
	}		
}

void ItemManager::on_popup_key_pressed( Popup* popup )
{
	// Nothing.
}

void ItemManager::on_popup_key_released( Popup* popup )
{
	// Exit application if error is killed.
	if (popup == error_) {
		if (error_->get_state() == POPUP_STATE_KILLED) {
			// Update if that's the reason we're exiting.
			if (update_error_) {
				launch_updater();
			}

			exit_application();
		}
	}
}

bool ItemManager::create_layout( void )
{
	// Create the user layer.
	if (!JUTIL::BaseAllocator::allocate( &user_layer_ ))
	{
		return false;
	}
	user_layer_ = new (user_layer_) Container();
	user_layer_->set_size( get_width(), get_height() );
	if (!add( user_layer_ ))
	{
		JUTIL::BaseAllocator::destroy( user_layer_ );
		return false;
	}

	// Create layout.
	JUI::VerticalLayout* layout;
	if (!JUTIL::BaseAllocator::allocate( &layout )) {
		return false;
	}
	layout = new (layout) JUI::VerticalLayout( SPACING, JUI::ALIGN_LEFT );
	if (!user_layer_->add( layout )) {
		JUTIL::BaseAllocator::destroy( layout );
		return false;
	}

	// Create inventory view.
	if (!JUTIL::BaseAllocator::allocate( &inventory_view_ )) {
		return false;
	}
	inventory_view_ = new (inventory_view_) AnimatedBookView( inventory_book_,
		PAGE_SPACING,
		SLOT_SPACING );

	// Create excluded view.
	excluded_view_ = new SlotBookView( excluded_book_,
		PAGE_SPACING,
		SLOT_SPACING );

	// Create button layout.
	JUI::Texture *craft_texture = graphics_->get_texture( "img/manager/gear.png" );
	JUI::Texture *equip_texture = graphics_->get_texture( "img/manager/equip.png" );
	JUI::Texture *sort_texture = graphics_->get_texture( "img/manager/sort.png" );
	//Texture *delete_texture = graphics_->get_texture( "manager/delete" );

	// Create inventory buttons.
	craft_button_ = Button::create_icon_label_button( craft_texture, "craft" );
	equip_button_ = Button::create_icon_label_button( equip_texture, "equip" );
	sort_button_ = Button::create_icon_label_button( sort_texture, "sort" );
	//delete_button_ = Button::create_icon_label_button( delete_texture, "delete" );

	craft_button_->set_enabled( false );
	equip_button_->set_enabled( false );
	sort_button_->set_enabled( false );
	//delete_button_->set_enabled( false );
	
	// Create inventory button layout.
	JUI::HorizontalLayout* inventory_buttons = new JUI::HorizontalLayout( BUTTON_SPACING );
	inventory_buttons->add( craft_button_ );
	inventory_buttons->add( equip_button_ );
	inventory_buttons->add( sort_button_ );
	//inventoryButtons->add( delete_button_ );
	inventory_buttons->pack();

	// Create pages buttons/text.
	prev_button_ = Button::create_label_button( "<" );
	next_button_ = Button::create_label_button( ">" );
	page_display_ = new JUI::WrappedText( page_font_, PAGE_LABEL_WIDTH );
	page_display_->set_colour( PAGE_LABEL_COLOUR );
	page_display_->set_text_formatting( DT_CENTER );
	update_page_display();

	// Create pages buttons layout.
	JUI::HorizontalLayout* pageButtons = new JUI::HorizontalLayout( BUTTON_SPACING );
	pageButtons->add( prev_button_ );
	pageButtons->add( page_display_ ); // PAGE LABEL, RATHER
	pageButtons->add( next_button_ );
	pageButtons->pack();

	HorizontalSplitLayout* button_layout = new HorizontalSplitLayout( inventory_view_->get_width() );
	button_layout->add_left( inventory_buttons);
	button_layout->add_right( pageButtons );
	button_layout->pack();
		
	// Create title.
	stringstream titleStream;
	titleStream << APPLICATION_TITLE << " " << APPLICATION_VERSION;

	// Add version number.
	JUI::Text* titleText = new JUI::Text( title_font_ );
	titleText->set_text( titleStream.str() );
	titleText->set_colour( TITLE_COLOUR );

	// Organize layout.
	layout->add( titleText );
	layout->add( inventory_view_ );
	layout->add( button_layout );
	layout->add( excluded_view_ );
	layout->pack();
	layout->set_position( (get_width() - layout->get_width()) / 2.0f, 
		(get_height() - layout->get_height()) / 2.0f );
	user_layer_->add( layout );

	// Create item display.
	item_display_ = new ItemDisplay();
	user_layer_->add( item_display_ );

	// Create popup view on top.
	popups_ = new PopupDisplay();
	popups_->set_size( get_width(), get_height() );
	popups_->set_popup_handler( this );
	add( popups_ );
}