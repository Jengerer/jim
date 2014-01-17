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

#include "http_resource_loader.hpp"
#include "resource.h"
#include "item_manager.hpp"

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#endif

// Temporarily disable equip and sort.
#define EQUIP_NOT_IMPLEMENTED
#define SORT_NOT_IMPLEMENTED

// Application attributes.
const JUTIL::ConstantString APPLICATION_TITLE = "Jengerer's Item Manager";
const JUTIL::ConstantString APPLICATION_VERSION = "0.9.9.9.8.1";
const int APPLICATION_WIDTH	= 900;
const int APPLICATION_HEIGHT = 540;

// Updater resources.
const JUTIL::ConstantString UPDATER_PATH = "auto_updater.exe";

// UI attributes.
const unsigned int EXIT_BUTTON_PADDING	= 10;
const DWORD PAGE_DELAY_INTERVAL = 500;
const unsigned int DEFAULT_PAGE_COUNT = 6;
const unsigned int TRIAL_PAGE_COUNT = 1;

// Item manager resource loading URL.
const JUTIL::ConstantString MANAGER_ROOT_URL = "http://www.jengerer.com/item_manager";

// Frame rate limiters.
const DWORD FRAME_SPEED = 1000 / 60;

/*
 * Item manager constructor.
 */
ItemManager::ItemManager( HINSTANCE instance ) : Application( instance )
{
    JUTIL::AllocationManager* manager =JUTIL::AllocationManager::get_instance();
    // manager->set_debug_break( 3733 );

	// Set application size.
	set_size( APPLICATION_WIDTH, APPLICATION_HEIGHT );

	// Set window parameters.
	JUI::Window* window = get_window();
	window->set_border( true );
	window->set_fullscreen( false );
	window->set_icon( IDI_ICON1 );

	// Threaded loader.
	definition_loader_ = nullptr;

    // Web interface.
    site_loader_ = nullptr;

	// Set inventory event handler.
    inventory_.set_listener( this );

	// Set default running function.
	set_think( &ItemManager::waiting_for_items );
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
    if (!title_.write( "%s (%s)", APPLICATION_TITLE.get_string(), APPLICATION_VERSION.get_string() )) {
        stack->log( "Failed to allocation application title." );
        return PrecacheResourcesFailure;
    }
    window_.set_title( &title_ );
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

    // Create UI view and load necessary resources.
    if (!JUTIL::BaseAllocator::allocate( &view_ )) {
        stack->log( "Failed to allocate item manager view object." );
        return PrecacheResourcesFailure;
    }
    new (view_) ItemManagerView( &inventory_, &steam_items_ );
	view_->set_size( APPLICATION_WIDTH, APPLICATION_HEIGHT );
    view_->set_listener( this );
    if (!add( view_ )) {
        stack->log( "Failed to add item manager view object to application." );
        return PrecacheResourcesFailure;
    }
    if (!view_->download_resources( site_loader_ )) {
        return PrecacheResourcesFailure;
    }

    // Finish loading button and notice fonts so we can initialize view.
	if (!Notice::precache( &graphics_ )) {
        return PrecacheResourcesFailure;
    }
	else if (!Button::precache( &graphics_ )) {
        return PrecacheResourcesFailure;
    }

    // Initialize the view.
    if (!view_->initialize()) {
        return PrecacheResourcesFailure;
    }

    // Base resources successful; load extra resources.
    if (!create_resources()) {
        const JUTIL::String* top = stack->get_top_error();
        if (!view_->create_error( top )) {
            return PrecacheResourcesFailure;
        }
		set_think( &ItemManager::exiting );
    }

	// Generate non-base layout.
	if (!view_->create_layout( &graphics_ )) {
		const JUTIL::String* top = stack->get_top_error();
		if (!view_->create_error( top )) {
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

    // TASK: verify no other steam API application is running.

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

    // Initialize steam.
	if (!steam_items_.load_interfaces()) {
		return false;
	}
	steam_items_.set_listener( &inventory_ );

	// Show progress notice.
    const JUTIL::ConstantString PREPARING_MESSAGE = "Waiting for inventory message from Steam...";
    if (!view_->set_loading_notice( &PREPARING_MESSAGE )) {
        stack->log( "Failed to create loading notice." );
        return false;
    }

    return true;
}

void ItemManager::close_interfaces( void )
{
    // Clean up view resources.
    if (view_ != nullptr) {
        view_->clean_up();
    }

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
        if (!view_->create_error( error )) {
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

/*
 * Handle callbacks until inventory loaded event comes.
 */
bool ItemManager::waiting_for_items( void )
{
	if (!steam_items_.handle_callbacks()) {
		return false;
	}
	return true;
}

/*
 * Handle definition load state.
 */
bool ItemManager::loading_schema( void )
{
	// Stack for reporting and getting.
	JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

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
			// Remove threaded loader.
			JUTIL::BaseAllocator::safe_destroy( &definition_loader_ );

            // Finished loading!
            view_->destroy_loading_notice();
			if (!inventory_.on_schema_loaded( &graphics_ )) {
				stack->log( "Failed to run post-schema load inventory event." );
				return false;
			}
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
            if (!view_->set_loading_notice( progress_message )) {
				stack->log( "Failed to set progress message for loader." );
				return false;
			}
			break;
		}
	}

    // No problems.
    return true;
}

bool ItemManager::running( void )
{
	if (!steam_items_.handle_callbacks()){
		return false;
	}
	if (!view_->on_enter_frame()) {
		return false;
	}
    return true;
}

bool ItemManager::exiting( void )
{
	// Just wait for exit.
    return true;
}

/*
 * Handle error acknowledge event from view.
 */
bool ItemManager::on_error_acknowledged( void )
{
    // TODO: if this error is update-related, launch it here.
    exit_application();
    set_think( &ItemManager::exiting );
    return true;
}

/*
 * Handle inventory load completion event.
 */
bool ItemManager::on_inventory_loaded( void )
{
	// Create thread to load item definitions.
	JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();
	if (!start_definition_load()) {
		return false;
	}

	// Update page display.
	if (!view_->update_page_display()) {
		stack->log( "Failed to update page display." );
		return false;
	}
	set_think( &ItemManager::loading_schema );
	return true;
}

/*
 * Handle item update event and trigger Steam message.
 */
bool ItemManager::on_item_moved( Item* item )
{
	// Just delegate to Steam to update backend.
	if (!steam_items_.update_item( item )) {
		return false;
	}
	return true;
}

JUI::IOResult ItemManager::on_mouse_clicked( JUI::Mouse* mouse )
{
    // Pass message to item manager view.
    JUI::IOResult result = view_->on_mouse_clicked( mouse );
    return result;
}

JUI::IOResult ItemManager::on_mouse_released( JUI::Mouse* mouse )
{
    // Pass message to item manager view.
    JUI::IOResult result = view_->on_mouse_released( mouse );
    return result;
}

JUI::IOResult ItemManager::on_mouse_moved( JUI::Mouse* mouse )
{
	// Pass message to item manager view.
    JUI::IOResult result = view_->on_mouse_moved( mouse );
    return result;
}

/*
 * Handle keyboard pressed input.
 */
JUI::IOResult ItemManager::on_key_pressed( int key )
{
	JUI::IOResult result = view_->on_key_pressed( key );
	if (result != JUI::IO_RESULT_HANDLED) {
		return result;
	}
	return JUI::IO_RESULT_UNHANDLED;
}

/*
 * Handle keyboard release input.
 */
JUI::IOResult ItemManager::on_key_released( int key )
{
	JUI::IOResult result = view_->on_key_released( key );
	if (result != JUI::IO_RESULT_UNHANDLED) {
		return result;
	}
	return JUI::IO_RESULT_UNHANDLED;
}

bool ItemManager::start_definition_load( void )
{
	// Set the message and redraw.
    const JUTIL::ConstantString LOADING_DEFINITION_MESSAGE = "Loading item definitions...";
    view_->set_loading_notice( &LOADING_DEFINITION_MESSAGE );

	// Set up loader.
    // TODO: Don't need to allocate this; can use the "done" flag in loader instead of checking pointer null.
    if (!JUTIL::BaseAllocator::allocate( &definition_loader_ )) {
        return false;
    }
    ItemSchema* schema = inventory_.get_schema();
    NotificationQueue* notifications = view_->get_notification_queue();
    definition_loader_ = new (definition_loader_) DefinitionLoader( &graphics_, schema, notifications );
    if (!definition_loader_->begin()) {
        return false;
    }
    return true;
}
