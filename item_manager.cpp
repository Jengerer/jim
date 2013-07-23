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
#include "resource.h"
#include "item_manager.hpp"

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#endif

// Temporarily disable equip and sort.
#define EQUIP_NOT_IMPLEMENTED
#define SORT_NOT_IMPLEMENTED

const int APPLICATION_WIDTH	= 965;
const int APPLICATION_HEIGHT = 540;

// Updater resources.
const JUTIL::ConstantString UPDATER_PATH = "auto_updater.exe";

// UI attributes.
const unsigned int EXIT_BUTTON_PADDING	= 10;
const DWORD PAGE_DELAY_INTERVAL			= 500;

// Item manager resource loading URL.
const JUTIL::ConstantString MANAGER_ROOT_URL = "http://www.jengerer.com/item_manager";

// Frame rate limiters.
const DWORD FRAME_SPEED = 1000 / 60;

/*
 * Item manager constructor.
 */
ItemManager::ItemManager( HINSTANCE instance )
    : Application( instance ),
      inventory_book_( INVENTORY_PAGE_SIZE ),
      excluded_book_( EXCLUDED_PAGE_SIZE ),
      inventory_( &inventory_book_, &excluded_book_ )
{
    JUTIL::AllocationManager* manager =JUTIL::AllocationManager::get_instance();
    // manager->set_debug_break( 3733 );

	// Set application size.
	set_size( APPLICATION_WIDTH, APPLICATION_HEIGHT );

	// Set window parameters.
	JUI::Window* window = get_window();
	window->set_title( APPLICATION_TITLE );
	window->set_border( true );
	window->set_fullscreen( false );
	window->set_icon( IDI_ICON1 );

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

    // Web interface.
    site_loader_ = nullptr;

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
	if (downloader == nullptr) {
		stack->log( "Failed to create downloader object." );
		return PrecacheResourcesFailure;
	}
	if (!JUTIL::BaseAllocator::allocate( &site_loader_ )) {
		stack->log( "Failed to allocate site loader object." );
		return PrecacheResourcesFailure;
	}
	site_loader_ = new (site_loader_) HttpResourceLoader( &MANAGER_ROOT_URL, downloader );

	// Get texture and font resources.
	bool success = site_loader_->get_resource( &EQUIP_ICON_TEXTURE, &EQUIP_ICON_TEXTURE ) &&
		site_loader_->get_resource( &CRAFT_ICON_TEXTURE, &CRAFT_ICON_TEXTURE ) &&
		site_loader_->get_resource( &SORT_ICON_TEXTURE, &SORT_ICON_TEXTURE ) &&
		site_loader_->get_resource( &UNKNOWN_ITEM_ICON_TEXTURE, &UNKNOWN_ITEM_ICON_TEXTURE ) &&
		site_loader_->get_resource( &TF2_BUILD_FONT, &TF2_BUILD_FONT ) &&
		site_loader_->get_resource( &TF2_SECONDARY_FONT, &TF2_SECONDARY_FONT );
	if (!success) {
		return PrecacheResourcesFailure;
	}

	// Necessary to display progress/status.
	if (!Notice::precache( &graphics_ )) {
        return PrecacheResourcesFailure;
    }
	else if (!Button::precache( &graphics_ )) {
        return PrecacheResourcesFailure;
    }

	// Create layers.
	if (!create_layers()) {
		stack->log( "Failed to create UI layers." );
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
		set_think( &ItemManager::exiting );
    }

    // All base resources loaded successfully.
    return Success;
}

bool ItemManager::create_resources( void )
{
    // Stack for logging.
    JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

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

    // Enough resources loaded to show styled messages.
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
	notifications_->set_position( get_width() - PADDING, get_height() - PADDING );
	if (!add( notifications_ )) {
        JUTIL::BaseAllocator::destroy( notifications_ );
        stack->log( "Failed to add notification queue!" );
        return false;
    }

    // Initialize steam.
	if (!steam_items_.load_interfaces()) {
		return false;
	}

	// Show progress notice.
    const JUTIL::ConstantString PREPARING_MESSAGE = "Waiting for inventory message from Steam...";
	load_progress_ = popups_->create_notice( &PREPARING_MESSAGE );
    if (load_progress_ == nullptr) {
        stack->log( "Failed to create loading notice." );
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

JUI::Application::ReturnStatus ItemManager::run( void )
{
    // Base application running and IO handling.
    JUI::Application::ReturnStatus result = Application::run();
    if (result != Success) {
        return result;
    }
	
    // Attempt to think; print error if failed.
    if (!think()) {
        // Create error message.
        JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();
        const JUTIL::String* error = stack->get_top_error();
        error_ = popups_->create_alert( error );

        // Check if failed to display error.
        if (error_ == nullptr) {
            return NoMemoryFailure;
        }
        set_think( &ItemManager::exiting );
    }
	draw_frame();
    return Success;
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
	// Stack for reporting and getting.
	JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

	// Wait for item loading if definition loading hasn't started.
    if (definition_loader_ == nullptr) {
		if(!handle_callback()){
			return false;
		}

        // Have items been loaded?
		if (backpack_ != nullptr && backpack_->is_loaded()) {
            // Start loading item definitions.
	        if (!start_definition_load()) {
                stack->log( "Failed to start loading item definitions." );
                return false;
            }
        }
    }
    else {
        // Parse definition loading state.
		switch (definition_loader_->get_state()) {
			case LOADING_STATE_ERROR:
			{
				// Remove threaded loader.
				JUTIL::BaseAllocator::safe_destroy( &definition_loader_ );
				return false;
			}

			case LOADING_STATE_FINISHED:
			{
				// Resolve all item definitions.
				schema_.set_loaded( true );
				if (!backpack_->resolve_definitions( &schema_, definition_loader_ )) {
					return false;
				}

				// Remove threaded loader.
				JUTIL::BaseAllocator::safe_destroy( &definition_loader_ );

				// Generate UI.
				if (!create_layout()) {
					return false;
				}

                // Finished loading!
                popups_->remove_popup( load_progress_ );
                set_think( &ItemManager::running );
				break;
			}

			default:
			{
                // Print message.
				const JUTIL::String* progress_message = definition_loader_->get_progress_message();
				if (progress_message == nullptr) {
					stack->log( "Failed to get progress message from loader." );
					return false;
				}
				if (!load_progress_->set_message( progress_message )) {
					stack->log( "Failed to set progress message for loader." );
					return false;
				}
				load_progress_->center_to( this );
				break;
			}
		}
	}

    // No problems.
    return true;
}

bool ItemManager::running( void )
{
	if(!handle_callback()){
		return false;
	}
	notifications_->update_notifications();
	update_item_display();
    return true;
}

bool ItemManager::exiting( void )
{
	// Just wait for exit.
    return true;
}

JUI::IOResult ItemManager::on_mouse_clicked( JUI::Mouse* mouse )
{
	// Popups take priority.
	JUI::IOResult result = popups_->on_mouse_clicked( mouse );
    if (result != JUI::IO_RESULT_UNHANDLED) {
        return result;
    }

    // No clicking allowed when dragging.
	if (dragged_view_ != nullptr) {
		return JUI::IO_RESULT_HANDLED;
	}

	// Find slot view if touched.
    unsigned int index;
    Item* item = nullptr;
	const SlotView* slot_view = nullptr;
	if (inventory_view_->get_touching_index( mouse, &index )) {
        item = inventory_book_->get_item( index );
        slot_view = inventory_view_->get_slot_view( index );
	}
	else if (excluded_view_->get_touching_index( mouse, &index )) {
        item = excluded_book_->get_item( index );
		slot_view = excluded_view_->get_slot_view( index );
	}
	if (slot_view != nullptr) {
        // Handle slot clicking if slot found.
		if (!on_slot_clicked( slot_view, item, mouse )) {
            return JUI::IO_RESULT_ERROR;
        }
		return JUI::IO_RESULT_HANDLED;
	}

    // Touch buttons.
    // TODO: Make a vector of these buttons; this shit is unwieldy.
	// TODO: Set a 'clicked target' pointer so we can't just release on button and trigger.
    result = craft_button_->on_mouse_clicked( mouse );
    if (result != JUI::IO_RESULT_UNHANDLED) {
        return result;
    }
    result = equip_button_->on_mouse_clicked( mouse );
    if (result != JUI::IO_RESULT_UNHANDLED) {
        return result;
    }
    result = sort_button_->on_mouse_clicked( mouse );
    if (result != JUI::IO_RESULT_UNHANDLED) {
        return result;
    }
    result = next_button_->on_mouse_clicked( mouse );
    if (result != JUI::IO_RESULT_UNHANDLED) {
        return result;
    }
    result = prev_button_->on_mouse_clicked( mouse );
    if (result != JUI::IO_RESULT_UNHANDLED) {
        return result;
    }
    result = craft_button_->on_mouse_clicked( mouse );
    if (result != JUI::IO_RESULT_UNHANDLED) {
        return result;
    }

    // Handle notification killing.
	result = notifications_->on_mouse_clicked( mouse );
    if (result != JUI::IO_RESULT_UNHANDLED) {
		return result;
	}

	// Nothing clicked; deselect items if not multi-selecting.
	if (steam_items_.get_select_mode() != SELECT_MODE_MULTIPLE) {
		steam_items_.deselect_all();
		update_buttons();
	}
	return JUI::IO_RESULT_UNHANDLED;
}

JUI::IOResult ItemManager::on_mouse_released( JUI::Mouse* mouse )
{
	// Check top popup.
    JUI::IOResult result = popups_->on_mouse_released( mouse );
    if (result != JUI::IO_RESULT_UNHANDLED) {
        return result;
    }

	// Release slot if dragging.
	if (dragged_view_ != nullptr) {
		SlotView* slot_view = inventory_view_->get_touching_slot( mouse );
		if (!on_slot_released( slot_view )) {
            return JUI::IO_RESULT_ERROR;
        }
        return JUI::IO_RESULT_HANDLED;
	}
	if (craft_button_->on_mouse_released( mouse ) == JUI::IO_RESULT_HANDLED) {
		if (steam_items_.is_selected_tradable()) {
			steam_items_.craft_selected();
			return JUI::IO_RESULT_HANDLED;
		}
		else {
			//craft_check_ = popups_->create_confirmation( "One or more of the items you've selected are not tradable. The result will not be tradable. Continue?" );
		}
	}
	else if (next_button_->on_mouse_released( mouse ) == JUI::IO_RESULT_HANDLED) {
		if (inventory_view_->next_page()) {
		    if (!update_page_display()) {
                result = JUI::IO_RESULT_ERROR;
            }
			else {
				return JUI::IO_RESULT_HANDLED;
			}
        }
	}
	else if (prev_button_->on_mouse_released( mouse ) == JUI::IO_RESULT_HANDLED) {
		if (inventory_view_->previous_page()) {
		    if (!update_page_display()) {
                result = JUI::IO_RESULT_ERROR;
            }
			else {
				return JUI::IO_RESULT_HANDLED;
			}
        }
	}

    return JUI::IO_RESULT_UNHANDLED;
}

JUI::IOResult ItemManager::on_mouse_moved( JUI::Mouse* mouse )
{
	// Update buttons.
	// TODO: Have a button frame mouse hover state.
	if (item_display_ != nullptr) {
		item_display_->set_item( nullptr );
	}

	// Pass to highest popup.
	JUI::IOResult result = popups_->on_mouse_moved( mouse );
    if (result != JUI::IO_RESULT_UNHANDLED) {
        return result;
	}
	
	// Check if dragging.
	if (dragged_view_ != nullptr) {
		result = dragged_view_->on_mouse_moved( mouse );
        if (result == JUI::IO_RESULT_ERROR) {
            return result;
        }
		clamp_child( dragged_view_ );

		// Check if we're switching page.
		DWORD current_tick = GetTickCount();
		if (current_tick > page_delay_) {
            // Check left page.
			if (dragged_view_->get_x() <= 0.0f) {
                // Update delay if we moved.
				if (inventory_view_->previous_page()) {
					page_delay_ = current_tick + PAGE_DELAY_INTERVAL;
                    update_page_display();
				}
			}
			else if (dragged_view_->get_x() + dragged_view_->get_width() >= get_width()) {
                // Update delay if we moved.
				if (inventory_view_->next_page()) {
					page_delay_ = current_tick + PAGE_DELAY_INTERVAL;
                    update_page_display();
				}
			}
		}	

        // Can't handle anything else while dragging.
        return JUI::IO_RESULT_HANDLED;
	}

	// Check if we've hovering over an slot view.
	Item* touched_item = nullptr;
	const SlotView* touched_view;
    unsigned int touched_index;
	if (inventory_view_ != nullptr) {
		// Get the touched item from inventory view, if any.
		if (mouse->is_touching( inventory_view_ ) && inventory_view_->get_touching_index( mouse, &touched_index )) {
			touched_item = inventory_book_->get_item_from_page( touched_index );
			touched_view = inventory_view_->get_slot_view( touched_index );
		}
	}
	else if (excluded_view_ != nullptr) {
		// Get the touched item from excluded view, if any.
		if (mouse->is_touching( excluded_view_ ) && excluded_view_->get_touching_index( mouse, &touched_index )) {
			touched_item = excluded_book_->get_item_from_page( touched_index );
			touched_view = inventory_view_->get_slot_view( touched_index );
		}
	}

	// Update the display with the new item.
	if (touched_item != nullptr) {
		if (!item_display_->set_item( touched_item )) {
            return JUI::IO_RESULT_ERROR;
        }

        // Position display; move above mouse if hitting bottom.
		int display_x = touched_view->get_x() + (touched_view->get_width() - item_display_->get_width()) / 2;
		int display_y = touched_view->get_y() + touched_view->get_height() + ITEM_DISPLAY_SPACING;
		if (display_y + item_display_->get_height() > get_height()) {
			display_y = touched_view->get_y() - item_display_->get_height() - ITEM_DISPLAY_SPACING;
		}

        // Position and clamp.
		item_display_->set_position( display_x, display_y );
		clamp_child( item_display_, PADDING );
        return JUI::IO_RESULT_HANDLED;
	}

	// Nothing left.
    return JUI::IO_RESULT_UNHANDLED;
}

/*
 * Handles item slot click events.
 *
 * slot_view - The slot view being clicked (guaranteed non-null).
 * item      - The item pointer if the clicked slot contains one (null otherwise).
 * mouse     - The mouse handle for this event.
 */
bool ItemManager::on_slot_clicked( const SlotView* slot_view, Item* item, JUI::Mouse* mouse )
{
    return true;
}

/*
 * Handles slot release events.
 *
 * slot_view - The slot view being clicked (guaranteed non-null).
 * item      - The item pointer if the clicked slot contains one (null otherwise).
 */
bool ItemManager::on_slot_released( const SlotView* slot_view, Item* item )
{
	return true;
}

void ItemManager::update_buttons()
{
#ifndef EQUIP_NOT_IMPLEMENTED
	equip_button_->set_enabled( steam_items_.can_equip_selected() );
#endif
}

/*
 * Handle keyboard pressed input.
 */
JUI::IOResult ItemManager::on_key_pressed( int key )
{
	// Now handle ourselves.
    JUI::IOResult result = JUI::IO_RESULT_HANDLED;
	if (inventory_view_ == nullptr) {
		return JUI::IO_RESULT_ERROR;
	}

	switch (key) {
    // Pass enter to popups.
	case VK_RETURN:
		result = popups_->on_key_pressed( key );
		break;

    // Toggle select mode with control.
	case VK_CONTROL:
		steam_items_.set_select_mode( SELECT_MODE_MULTIPLE );
		break;

	// Toggle fask skip mode with shift
	case VK_SHIFT:
		inventory_view_->set_shift_multiple( SLOT_BOOK_VIEW_SHIFT_ON );
		break;

    // Switch to left page.
	case VK_LEFT:
	case VK_UP:
	case 'A':
	case 'W':
		if (inventory_view_->previous_page()) {
		    if (!update_page_display()) {
                result = JUI::IO_RESULT_ERROR;
            }
        }
		break;

    // Switch to right page.
	case VK_RIGHT:
	case VK_DOWN:
	case 'D':
	case 'S':
		if (inventory_view_->next_page()) {
		    if (!update_page_display()) {
                result = JUI::IO_RESULT_ERROR;
            }
        }
		break;

	// Switch to first page
	case VK_HOME:
		if (inventory_view_->first_page()) {
		    if (!update_page_display()) {
                result = JUI::IO_RESULT_ERROR;
            }
        }
		break;

	// Switch to last page
	case VK_END:
		if (inventory_view_->last_page()) {
		    if (!update_page_display()) {
                result = JUI::IO_RESULT_ERROR;
            }
        }
		break;

	case VK_OEM_3:
		if (inventory_view_->jump_to_page( key - VK_OEM_3 )) {
		    if (!update_page_display()) {
                result = JUI::IO_RESULT_ERROR;
            }
        }
		break;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		if (inventory_view_->jump_to_page( key - '0' )) {
		    if (!update_page_display()) {
                result = JUI::IO_RESULT_ERROR;
            }
        }
		break;
	/* NumPad does not work with shift
	case VK_NUMPAD0:
	case VK_NUMPAD1:
	case VK_NUMPAD2:
	case VK_NUMPAD3:
	case VK_NUMPAD4:
	case VK_NUMPAD5:
	case VK_NUMPAD6:
	case VK_NUMPAD7:
	case VK_NUMPAD8:
	case VK_NUMPAD9:
		if (inventory_view_->jump_to_page( key - VK_NUMPAD0 )) {
		    if (!update_page_display()) {
                result = JUI::IO_RESULT_ERROR;
            }
        }
		break;

	*/
    // Key event not handled.
    default:
        result = JUI::IO_RESULT_UNHANDLED;
        break;
	}

	return result;
}

/*
 * Handle keyboard release input.
 */
JUI::IOResult ItemManager::on_key_released( int key )
{
    JUI::IOResult result = JUI::IO_RESULT_HANDLED;
	switch (key) {
    // Toggle select mode with control release.
	case VK_CONTROL:
		steam_items_.set_select_mode( SELECT_MODE_SINGLE );
		break;

	// Toggle fask skip mode with shift
	case VK_SHIFT:
		inventory_view_->set_shift_multiple( SLOT_BOOK_VIEW_SHIFT_OFF );
		break;

    // Key event not handled.
    default:
        result = JUI::IO_RESULT_UNHANDLED;
        break;
	}

	return result;
}

/*
 * Handle mouse buttons.
 */
JUI::IOResult ItemManager::handle_button_released( JUI::Mouse* mouse )
{
    // Handle craft button.
    JUI::IOResult result = craft_button_->on_mouse_released( mouse );
    if (result == JUI::IO_RESULT_HANDLED) {
        // Craft if tradable.
		if (steam_items_.is_selected_tradable()) {
			steam_items_.craft_selected();
		}
		else {
            // Show warning if not tradable.
            const JUTIL::ConstantString TRADABLE_WARNING = "One or more of the items you've selected are not tradable. The result will not be tradable. Continue?";
			craft_check_ = popups_->create_confirmation( &TRADABLE_WARNING );
            if (craft_check_ != nullptr) {
                result = JUI::IO_RESULT_ERROR;
            }
		}
	}
    if (result != JUI::IO_RESULT_UNHANDLED) {
        return result;
    }

    // Handle next button.
	result = next_button_->on_mouse_released( mouse );
    if (result == JUI::IO_RESULT_HANDLED) {
		if (inventory_view_->next_page()) {
            // Try update page display.
		    if (!update_page_display()) {
                result = JUI::IO_RESULT_ERROR;
            }
        }
	}
    if (result != JUI::IO_RESULT_UNHANDLED) {
        return result;
    }

    // Handle previous button.
	result = prev_button_->on_mouse_released( mouse );
    if (result == JUI::IO_RESULT_HANDLED) {
	    if (inventory_view_->previous_page()) {
            // Try update page display.
            if (!update_page_display()) {
                result = JUI::IO_RESULT_ERROR;
			}
		}
	}
    if (result != JUI::IO_RESULT_UNHANDLED) {
        return result;
    }

    // Nothing handled yet.
    return JUI::IO_RESULT_UNHANDLED;
}

/*
 * Update item display frame state.
 */
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
    definition_loader_ = new (definition_loader_) DefinitionLoader( &graphics_, &schema_, notifications_ );
	if (!definition_loader_->begin()) {
        return false;
    }
    return true;
}

bool ItemManager::is_latest_version( void ) const
{
	// Do not check for updated when running debug builds
#if !defined( _DEBUG )
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
#endif

	// If failed to get version, assume latest.
	return true;
}

void ItemManager::launch_updater( void ) const
{
	// Get updater always.
	site_loader_->update_resource( &UPDATER_PATH, &UPDATER_PATH );

	// TODO: Make an error type enum and launch this on update error.
	int result = (int)ShellExecute( 0, 0, "auto_updater.exe", 0, 0, SW_SHOWDEFAULT );
	if (result <= 32) {
		MessageBox( nullptr, "Failed to run updater, try re-downloading the application manually.", "Automatic update error!", MB_ICONEXCLAMATION | MB_OK );
	}
}

/*
 * Handle a Steam callback waiting.
 * Returns true on successful handle, false otherwise.
 */
bool ItemManager::handle_callback( void )
{
	CallbackMsg_t callback;
	if (steam_items_.get_callback( &callback )) {
        // Check callback type.
		switch (callback.m_iCallback) {
		    case GCMessageAvailable_t::k_iCallback:
		    {
		        GCMessageAvailable_t *message = reinterpret_cast<GCMessageAvailable_t*>(callback.m_pubParam);

                // Get size of message waiting.
		        uint32 size;
		        if (steam_items_.has_message( &size )) {
			        uint32 id;
                    uint32 real_size;
                    JUTIL::ArrayBuilder<BYTE> buffer;
                    if (!buffer.set_size( size )) {
                        return false;
                    }

                    if (steam_items_.get_message( &id, buffer.get_array(), size, &real_size )) {
				        // Filter protobuf messages.
				        if ((id & 0x80000000) != 0) {
					        uint32 real_id = id & 0x0FFFFFFF;

					        // First get the protobuf struct header.
					        SerializedBuffer header_buffer( buffer.get_array() );
					        GCProtobufHeader_t* header_struct = header_buffer.get<GCProtobufHeader_t>();
					        uint32 header_size = header_struct->m_cubProtobufHeader;

					        // Now get the real protobuf header.
					        CMsgProtoBufHeader header_msg;
					        void* header_bytes = header_buffer.here();
					        if (!header_msg.ParseFromArray( header_bytes, header_size )) {
                                return false;
                            }
					        header_buffer.push( header_size );

					        // Check if we can set target ID.
					        // TODO: Maybe move all this horseshit into Steam.
					        if (header_msg.has_job_id_source()) {
						        steam_items_.set_target_id( header_msg.job_id_source() );
					        }

                            // Pass message to protobuf handler.
					        uint32 body_size = size - sizeof(GCProtobufHeader_t) - header_size;
					        if (!handle_protobuf( real_id, header_buffer.here(), body_size )) {
                                return false;
                            }
				        }
				        else {
					        if (!handle_message( id, buffer.get_array(), size )) {
                                return false;
                            }
				        }
			        }
		        }
	        }
	    }
    }

	steam_items_.release_callback();
    return true;
}

/*
 * Handle non-protobuf game coordinator message.
 */
bool ItemManager::handle_message( uint32 id, void* message, size_t size )
{
    // Get stack for reporting.
    JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

    // Handle by message ID.
	switch (id) {
		case GCCraftResponse_t::k_iMessage:
		{
            // Get response and display to user.
			GCCraftResponse_t* response = reinterpret_cast<GCCraftResponse_t*>(message);
			if (response->blueprint == 0xffff) {
				const JUTIL::ConstantString CRAFT_FAIL_MESSAGE = "Crafting failed: no such blueprint!";
				if (!notifications_->add_notification( &CRAFT_FAIL_MESSAGE, nullptr )) {
                    stack->log( "Failed to allocate craft response notification." );
                    return false;
                }
			}

			break;
		}
	}

    // No issues.
    return true;
}

/*
 * Handle protobuf message.
 */
bool ItemManager::handle_protobuf( uint32 id, void* message, size_t size )
{
    // Get error stack for logging.
    JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

    // Handle message from ID.
	switch (id) {
	case k_ESOMsg_CacheSubscribed:
		{
			// Get message.
			CMsgSOCacheSubscribed cache_msg;
			if (!cache_msg.ParseFromArray( message, size )) {
                return false;
            }
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
                            // Get data.
                            const char* data = subscribed_type.object_data( i ).data();
                            size_t size = subscribed_type.object_data( i ).size();
                            CSOEconItem econ_item;
							if (!econ_item.ParseFromArray( data, size )) {
                                stack->log( "Failed to parse item object from Steam message." );
                                return false;
                            }

                            // Get item from protobuf.
                            Item* item = create_item_from_message( &econ_item );
                            if (item == nullptr) {
                                stack->log( "Failed to create item from Steam object." );
                                return false;
                            }

                            // Add to backpack.
							if (!backpack_->add_item( item )) {
                                JUTIL::BaseAllocator::destroy( item );
                                stack->log( "Failed to add item to inventory." );
                                return false;
                            }
						}

						break;
					}
				case 7:
					{
						if (steam_items_.get_steam_id() == cache_msg.owner()) {
							for (int i = 0; i < subscribed_type.object_data_size(); i++) {
								CSOEconGameAccountClient client;
                                const char* data = subscribed_type.object_data( i ).data();
                                size_t size = subscribed_type.object_data( i ).size();
								if (!client.ParseFromArray( data, size )) {
                                    stack->log( "Failed to parse account client object from Steam message." );
                                    return false;
                                }

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
                                    stack->log( "New backpack size smaller than old." );
                                    return false;
								}

								// Add the newer slots.
								unsigned int added_slots = slots - inventory_book_->get_slot_count();
								unsigned int added_pages = added_slots / inventory_book_->get_page_size();
								inventory_book_->add_pages( added_pages );
							}
						}

						break;
					}
				}
			}

			// Update view if available.
			if (inventory_view_ != nullptr) {
				if (!inventory_view_->update_pages()) {
					stack->log( "Failed to update UI for added pages." );
					return false;
				}

				if (!update_page_display()) {
					stack->log( "Failed to update page display." );
					return false;
				}
			}

			backpack_->resolve_excluded();
			backpack_->set_loaded( true );

			// Create layout if this is the first cache.
            const JUTIL::ConstantString LOAD_MESSAGE = "Backpack successfully loaded from Steam.";
			if (!notifications_->add_notification( &LOAD_MESSAGE, nullptr )) {
                stack->log( "Failed to add backpack load notification." );
                return false;
            }

			break;
		}

	case 27:
		{
			CMsgSOCacheSubscriptionCheck check;
			if (!check.ParseFromArray( message, size )) {
                stack->log( "Failed to parse inventory subscription check." );
                return false;
            }

			// Compare version.
			uint64 version = check.version();
			if (steam_items_.get_version() != version) {
				steam_items_.set_version( version );

				// Send refresh.
				CMsgSOCacheSubscriptionRefresh refresh;
				refresh.set_owner( steam_items_.get_steam_id() );

                // TODO: How do we serialize without string?
				std::string refreshString = refresh.SerializeAsString();
				if (!steam_items_.send_message( 28 | 0x80000000, (void*)refreshString.c_str(), refreshString.size() )) {
                    stack->log( "Failed to send inventory subscription refresh to Steam." );
                    return false;
                }
			}
			break;
		}

	case k_ESOMsg_Update:
		{
			CMsgSOSingleObject update_msg;
			if (!update_msg.ParseFromArray( message, size )) {
                stack->log( "Failed to parse item update message from Steam." );
                return false;
            }
			steam_items_.set_version( update_msg.version() );

			if (update_msg.type_id() == 1) {
				CSOEconItem updated_item;
				if (!updated_item.ParseFromArray( update_msg.object_data().data(), update_msg.object_data().size() )) {
                    stack->log( "Failed to parse item update message from Steam." );
                    return false;
                }

				// Find updated item.
				Item* target = backpack_->find_item( updated_item.id() );
				if (target == nullptr) {
					break;
				}

				// Place item into excluded, to be resolved later.
				backpack_->displace_item( target );
				target->set_inventory_flags( updated_item.inventory() );
				if (!backpack_->place_item( target )) {
                    stack->log( "Failed to update item position in backpack." );
                    return false;
                }
			}

			break;
		}

	case k_ESOMsg_UpdateMultiple:
		{
			CMsgSOMultipleObjects update_msg;
			if (!update_msg.ParseFromArray( message, size )) {
                stack->log( "Failed to parse multiple item update message from Steam." );
                return false;
            }
			steam_items_.set_version( update_msg.version() );

			for (int i = 0; i < update_msg.objects_size(); i++) {
				CMsgSOMultipleObjects::SingleObject current_object = update_msg.objects( i );
				if (current_object.type_id() == 1) {
					CSOEconItem updated_item;
					if (!updated_item.ParseFromArray( current_object.object_data().data(), current_object.object_data().size() )) {
                        stack->log( "Failed to parse multiple item message from Steam." );
                        return false;
                    }

					// Attempt to find the item.
					Item* target = backpack_->find_item( updated_item.id() );
					if (target != nullptr) {
						// TODO: bump old into excluded.
						backpack_->displace_item( target );
						target->set_inventory_flags( updated_item.inventory() );
						if (!backpack_->place_item( target )) {
                            stack->log( "Failed to update multiple items from Steam message." );
                            return false;
                        }
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
			if (!create_msg.ParseFromArray( message, size )) {
                stack->log( "Failed to parse item creation message from Steam." );
                return false;
            }
			steam_items_.set_version( create_msg.version() );

			// Get item from object.
			CSOEconItem created_item;
			if (!created_item.ParseFromArray( create_msg.object_data().data(), create_msg.object_data().size() )) {
                stack->log( "Failed to parse created item from Steam message." );
                return false;
            }

			// Now add item.
            Item* item = create_item_from_message( &created_item );
            if (item == nullptr) {
                stack->log( "Failed to create new item from Steam message." );
                return false;
            }

			// Add this item to excluded.
			if (!backpack_->add_item( item )) {
                JUTIL::BaseAllocator::destroy( item );
                stack->log( "Failed to insert new item into backpack." );
                return false;
            }
			
			// Display message if schema loaded.
			if (schema_.is_loaded()) {
				// Get the source.
				const JUTIL::ConstantString CRAFTED_SOURCE = "crafted";
				const JUTIL::ConstantString FOUND_SOURCE = "found";
				const JUTIL::String* source;
				if (created_item.origin() == 4) {
					source = &CRAFTED_SOURCE;
				}
				else {
					source = &FOUND_SOURCE;
				}

				JUTIL::DynamicString message;
				if (!message.write( "You have %s a %s.", source->get_string(), item->get_name()->get_string() )) {
					stack->log( "Failed to create new item message." );
					return false;
				}
				if (!notifications_->add_notification( &message, item->get_texture(0) )) {
					stack->log( "Failed to add new item message to notifications." );
					return false;
				}
			}

			break;
		}

	case SOMsgDeleted_t::k_iMessage:
		{
			// Get deleted message.
			CMsgSOSingleObject delete_msg;
			if (!delete_msg.ParseFromArray( message, size )) {
                stack->log( "Failed to parse item deletion message from Steam." );
                return false;
            }
			steam_items_.set_version( delete_msg.version() );

			// Get ID of deleted item.
			CSOEconItem deleted_item;
			if (!deleted_item.ParseFromArray( delete_msg.object_data().data(), delete_msg.object_data().size() )) {
                stack->log( "Failed to parse deleted item object from Steam." );
                return false;
            }

			// Now remove from inventory.
			Item* target = backpack_->find_item( deleted_item.id() );
			if (target != nullptr) {
				backpack_->remove_item( target );

				// Make sure the deleted item isn't being dragged.
				if (dragged_view_ != nullptr) {
					Slot* dragged_slot = dragged_view_->get_slot();
					if (dragged_slot->get_item() == target) {
						// TODO: Dragged dies many ways; make a function?
                        steam_items_.deselect_all();
						remove( dragged_view_ );
                        JUTIL::BaseAllocator::destroy( dragged_view_ );
						dragged_view_ = nullptr;
					}
				}
                JUTIL::BaseAllocator::destroy( target );
			}

			break;
		}

	default:
		break;
	}

    return true;
}

/*
 * Handle popup mouse press event.
 * Return true on successful handle, false otherwise.
 */
bool ItemManager::on_popup_clicked( Popup* popup )
{
	return true;
}

/*
 * Handle popup mouse release event.
 * Return true on successful handle, false otherwise.
 */
bool ItemManager::on_popup_released( Popup* popup )
{
	// Exited popup handling.
	if (popup->get_state() == POPUP_STATE_KILLED) {
        if (popup == error_) {
			// Update if that's the reason we're exiting.
			if (update_error_) {
				launch_updater();
			}

			exit_application();
        }
        // Craft confirmation dialog handling.
        else if (popup == craft_check_) {
			if (craft_check_->get_response() == RESPONSE_YES) {
				if (!steam_items_.craft_selected()) {
                    return false;
                }

				update_buttons();
			}
		}
	}
    return true;
}

/*
 * Handle popup keyboard press event.
 * Return true on successful handle, false otherwise.
 */
bool ItemManager::on_popup_key_pressed( Popup* popup )
{
	return true;
}

/*
 * Handle popup keyboard release event.
 * Return true on successful handle, false otherwise.
 */
bool ItemManager::on_popup_key_released( Popup* popup )
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

	return true;
}

/*
 * Generate an item from a JSON object.
 */
Item* ItemManager::create_item_from_message( CSOEconItem* econ_item )
{
	// Error stack for reporting.
	JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

	// Create item.
	Item* item;
	if (!JUTIL::BaseAllocator::allocate( &item )) {
		return nullptr;
	}
	item = new (item) Item(
		econ_item->id(),
		econ_item->def_index(),
		econ_item->level(),
		static_cast<EItemQuality>(econ_item->quality()),
		econ_item->quantity(),
		econ_item->inventory(),
		econ_item->origin()
		);

	// Check equip fields.
	int j;
	if (econ_item->contains_equipped_state()) {
		for (j = 0; j < econ_item->equipped_state_size(); ++j) {
			const CSOEconItemEquipped& equipped = econ_item->equipped_state( j );
			EEquipClass equip_class = (EEquipClass) equipped.new_class();
			EEquipSlot equip_slot = (EEquipSlot) equipped.new_slot();
			if( equip_class != EQUIP_CLASS_SCOUT_UNUSED && equip_slot != EQUIP_SLOT_PREVIOUS ){
				EquippedStatus* equipped_stat;
				if(!JUTIL::BaseAllocator::allocate( &equipped_stat )) {
					return nullptr;
				}
				equipped_stat = new (equipped_stat) EquippedStatus(
					(EEquipClass) equipped.new_class(),
					(EEquipSlot) equipped.new_slot()
					);
				if(!item->add_equipped_data( equipped_stat )) {
					JUTIL::BaseAllocator::destroy( equipped_stat );
					return nullptr;
				}
			}
		}
	}

	// Add the item's attributes.
	for (j = 0; j < econ_item->attribute_size(); ++j) {
		const CSOEconItemAttribute& attribute = econ_item->attribute( j );
		uint32 attrib_index = attribute.def_index();

		// Create attribute.
		AttributeValue value;
		value.as_uint32 = attribute.value();
		Attribute* new_attribute;
		if (!JUTIL::BaseAllocator::allocate( &new_attribute )) {
			JUTIL::BaseAllocator::destroy( item );
			stack->log( "Failed to allocate attribute for item." );
			return false;
		}
		new_attribute = new (new_attribute) Attribute( attrib_index, value );

		// Add to item.
		if (!item->add_attribute( new_attribute )) {
			JUTIL::BaseAllocator::destroy( new_attribute );
			JUTIL::BaseAllocator::destroy( item );
			stack->log( "Failed to add attribute to item." );
			return false;
		}
	}
	
	// Load item and attribute definitions.
    if (schema_.is_loaded()) {
        if (!schema_.resolve( item )) {
			JUTIL::BaseAllocator::destroy( item );
			return false;
		}

		// Finalize state.
		item->update_attributes();

		// Add alt textures
		if(!definition_loader_->get_alt_texture( item )){
			JUTIL::BaseAllocator::destroy( item );
			return false;
		}

	}

	// Generate name.
	if (econ_item->has_custom_name()) {
		const JUTIL::ConstantString name = econ_item->custom_name().c_str();
		if (!item->set_custom_name( &name )) {
			JUTIL::BaseAllocator::destroy( item );
			stack->log( "Failed to set custom name for item." );
			return false;
		}
	}

	return item;
}
