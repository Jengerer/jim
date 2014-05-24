#include <stdlib.h>
#include <crtdbg.h>
#include <fstream>
#include <math.h>
#include <windows.h>
#include <shellapi.h>
#include <json/json.h>
#include <jutil_version.hpp>
#include <jui/gfx/font_factory.hpp>
#include <jui/application/error_stack.hpp>
#include <jui/jui_version.hpp>
#include "serialized_buffer.hpp"
#include "slot_view.hpp"
#include "http_resource_loader.hpp"
#include "resource.h"
#include "item_manager.hpp"
#include "item_decorator.hpp"

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#endif

// Temporarily disable equip and sort.
#define EQUIP_NOT_IMPLEMENTED
#define SORT_NOT_IMPLEMENTED

// Application attributes.
const JUTIL::ConstantString APPLICATION_TITLE = "Jengerer's Item Manager";
const JUTIL::ConstantString APPLICATION_VERSION = "0.9.9.9.9.9.9.5.2";
const int APPLICATION_FRAMERATE = 60;
const long APPLICATION_FRAME_TIME = 1000 / APPLICATION_FRAMERATE;
const int APPLICATION_WIDTH	= 900;
const int APPLICATION_HEIGHT = 540;

// UI attributes.
const unsigned int EXIT_BUTTON_PADDING	= 10;
const DWORD PAGE_DELAY_INTERVAL = 500;
const unsigned int DEFAULT_PAGE_COUNT = 6;
const unsigned int TRIAL_PAGE_COUNT = 1;
const unsigned int TRIAL_SLOT_COUNT = 50;
const unsigned int PREMIUM_SLOT_COUNT = 300;

// Item manager resource loading URL.
const JUTIL::ConstantString MANAGER_ROOT_URL = "http://www.jengerer.com/item_manager/downloads";

/*
 * Item manager constructor.
 */
ItemManager::ItemManager( HINSTANCE instance )
	: Application( instance ),
	definition_loader_( nullptr ),
	site_loader_( nullptr ),
	updater_( nullptr ),
	is_pending_update_( false ),
	pending_deletes_( false ),
	think_function_( nullptr )
{
    JUTIL::AllocationManager* manager = JUTIL::AllocationManager::get_instance();
    // manager->set_debug_break( 3733 );

	// Set application size.
	set_size( APPLICATION_WIDTH, APPLICATION_HEIGHT );

	// Set window parameters.
	JUI::Window* window = get_window();
	window->set_border( true );
	window->set_fullscreen( false );
	window->set_icon( IDI_ICON1 );

	// Set inventory event handler.
    inventory_.set_listener( this );

	// No think function by default.
	set_think( nullptr );
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

	// Get downloader.
	JUI::FileDownloader* downloader = JUI::FileDownloader::get_instance();
	if (downloader == nullptr) {
		stack->log( "Failed to create downloader object." );
		return PrecacheResourcesFailure;
	}
	// Allocate site loader.
	if (!JUTIL::BaseAllocator::allocate( &site_loader_ )) {
		stack->log( "Failed to allocate site loader object." );
		return PrecacheResourcesFailure;
	}
	site_loader_ = new (site_loader_) HttpResourceLoader( &MANAGER_ROOT_URL, downloader );

	// Create updater and remove previous files.
	if (!JUTIL::BaseAllocator::allocate( &updater_ )) {
		stack->log( "Failed to allocate object for updater." );
		return PrecacheResourcesFailure;
	}
	new (updater_) Updater( downloader, site_loader_, &APPLICATION_VERSION );

	// Remove previous files.
	if (!updater_->remove_old_files()) {
		return PrecacheResourcesFailure;
	}

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

    // Create UI view and load necessary resources.
    if (!JUTIL::BaseAllocator::allocate( &view_ )) {
        stack->log( "Failed to allocate item manager view object." );
        return PrecacheResourcesFailure;
    }
    new (view_) ItemManagerView( &inventory_, &schema_ );
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

	// Check if update required.
#if !defined(_DEBUG)
	UpdateCheckResult result = updater_->check_version_numbers();
	if (result == CHECK_FAILED) {
		return PrecacheResourcesFailure;
	}
	else if (result == UPDATE_REQUIRED) {
		// Notify user of update.
		const JUTIL::ConstantString UPDATE_ALERT( "The item manager is out of date and needs to be updated against your will. Press \"okay\". You have no choice!" );
		if (!view_->create_error( &UPDATE_ALERT )) {
			stack->log( "Failed to create update alert. This is embarassing." );
			return PrecacheResourcesFailure;
		}
		set_think( &ItemManager::pending_update );
		is_pending_update_ = true;
	}
	else
#endif
	{
		// No update needed, load as normal.
		JUTIL::BaseAllocator::destroy( updater_ );
		updater_ = nullptr;

		// Base resources successful; load extra resources.
		if (!create_resources()) {
			const JUTIL::String* top = stack->get_top_error();
			if (!view_->create_error( top )) {
				return PrecacheResourcesFailure;
			}
			set_think( &ItemManager::exiting );
		}
		else {
			// Generate non-base layout.
			if (!view_->create_layout( &graphics_ )) {
				const JUTIL::String* top = stack->get_top_error();
				if (!view_->create_error( top )) {
					return PrecacheResourcesFailure;
				}
				set_think( &ItemManager::exiting );
			}

			// Start waiting for items to come in.
			const JUTIL::ConstantString PREPARING_MESSAGE = "Waiting for inventory message from Steam...";
			if (!view_->set_loading_notice( &PREPARING_MESSAGE )) {
				stack->log( "Failed to create loading notice." );
				return PrecacheResourcesFailure;
			}
			set_think( &ItemManager::waiting_for_items );
		}
	}

    // All base resources loaded successfully.
	next_frame_ = GetTickCount() + APPLICATION_FRAME_TIME;
    return Success;
}

/*
 * Submit pending updated items and wait for response.
 */
void ItemManager::exit_application( void )
{
	// Either no pending item updates or user got impatient.
	Application::exit_application();
	set_think( &ItemManager::exiting );
}

bool ItemManager::create_resources( void ){
    // Stack for logging.
    JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

    // TASK: verify no other steam API application is running.

	// Download resources for inventory display.
	if (!ItemDecorator::download_resources( site_loader_ )) {
		return false;
	}

    // Enough resources loaded to show styled messages.
	if (!ItemDecorator::precache( &graphics_ )) {
		return false;
	}
	if (!ItemDisplay::precache()) {
        return false;
    }
	else if (!Notification::precache()) {
        return false;
    }

	// Initialize steam.
	if (!steam_items_.load_interfaces()) {
		return false;
	}
	steam_items_.set_listener( this );

    return true;
}

void ItemManager::close_interfaces( void )
{
    JUTIL::BaseAllocator::safe_destroy( &definition_loader_ );
    JUTIL::BaseAllocator::safe_destroy( &site_loader_ );
	JUTIL::BaseAllocator::safe_destroy( &updater_ );

	// Free cached resources.
	ItemDecorator::release();

	// Close font library.
	JUI::FontFactory::shut_down();

	// Free all protobuf library resources.
	google::protobuf::ShutdownProtobufLibrary();

	// Close downloader.
	JUI::FileDownloader::shut_down();
}

/*
 * Start loading definitions for schema.
 */
bool ItemManager::start_definition_load( void )
{
	// Set the message and redraw.
    const JUTIL::ConstantString LOADING_DEFINITION_MESSAGE = "Loading item definitions...";
    view_->set_loading_notice( &LOADING_DEFINITION_MESSAGE );

	// Set up loader.
    if (!JUTIL::BaseAllocator::allocate( &definition_loader_ )) {
        return false;
    }
    NotificationQueue* notifications = view_->get_notification_queue();
    definition_loader_ = new (definition_loader_) DefinitionLoader( &graphics_, &schema_, notifications );
    if (!definition_loader_->begin()) {
        return false;
    }
    return true;
}

/*
 * Resolve item and attribute definitions from loaded schema
 * and place them into the inventory.
 */
bool ItemManager::on_schema_loaded( void )
{
	// Resolve each inventory item's definition.
	unsigned int i;
	unsigned int end = inventory_.get_item_count();
	for (i = 0; i < end; ++i) {
		Item* item = inventory_.get_item( i );
		if (!schema_.resolve( item, &graphics_ )) {
			return false;
		}

		// Place item into inventory.
		if (!inventory_.place_item( item )) {
			return false;
		}
	}

	// Update excluded page state.
	view_->update_excluded_page_display();
	return true;
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
	swap_buffers();

	// Sleep for remainder of slice.
	long time = static_cast<long>(GetTickCount());
	long remainder = next_frame_ - time;
	if (remainder > 0) {
		Sleep(remainder);
		next_frame_ += APPLICATION_FRAME_TIME;
	}
	else {
		// If we missed deadline, don't bother trying to catch up.
		next_frame_ = time + APPLICATION_FRAME_TIME;
	}
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
			if (!on_schema_loaded()) {
				stack->log( "Failed to run post-schema load event." );
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

/*
 * Waiting for user to trigger update.
 */
bool ItemManager::pending_update( void )
{
	return true;
}

/*
 * Waiting for update to complete.
 */
bool ItemManager::updating( void )
{
	// Stack for errors.
	JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

	// Don't try join until it's finished.
	UpdateStatus status = updater_->get_update_status();
	if (status != UPDATE_PENDING) {
		update_thread_->join();
		JUTIL::BaseAllocator::destroy( update_thread_ );

		// Check return value.
		if (status == UPDATE_FAILED) {
			return false;
		}

		// Run new item manager.
		if (!updater_->run_new_version()) {
			stack->log( "Failed to run the new version of the item manager. I fail miserably, try to run it yourself." );
			return false;
		}
		exit_application();
	}
	return true;
}

/*
 * Waiting for application to close.
 */
bool ItemManager::exiting( void )
{
    return true;
}

/*
 * Handle error acknowledge event from view.
 */
bool ItemManager::on_error_acknowledged( void )
{
	// Stack for reporting errors.
	JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

	// Check if we should update.
	if (is_pending_update_) {
		// Tell user we're updating now.
		const JUTIL::ConstantString UPDATING_NOTICE( "Updating the item manager..." );
		if (!view_->set_loading_notice( &UPDATING_NOTICE )) {
			stack->log( "Failed to create updating notice." );
			return false;
		}

		// Create thread for updating.
		if (!JUTIL::BaseAllocator::allocate( &update_thread_ )) {
			stack->log( "Failed to create updater thread." );
			return false;
		}
		new (update_thread_) boost::thread( boost::bind( &Updater::update, updater_ ) );
		set_think( &ItemManager::updating );
	}
	else {
		exit_application();
	}
    return true;
}

/*
 * Triggered when the view begins moving some items.
 */
void ItemManager::on_item_begin_move( void )
{
	steam_items_.clear_item_updates();
}

/*
 * Handle item update event and trigger Steam message.
 */
bool ItemManager::on_item_moved( Item* item, unsigned int index )
{
	// Send request to update position.
	inventory_.displace_item( item );
	inventory_.move_item( item, index );
	if (!steam_items_.add_item_update( item )) {
		return false;
	}
	return true;
}

/*
 * Triggered when the view is finished moving some items.
 */
bool ItemManager::on_item_end_move( void )
{
	if (!steam_items_.push_item_updates()) {
		return false;
	}
	return true;
}

/*
 * Handle item crafting.
 */
bool ItemManager::on_craft_items( void )
{
	JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

	const JUTIL::ConstantString CRAFT_ERROR( "Failed to send craft message." );
	DynamicSlotArray* selected = inventory_.get_selected_slots();
	unsigned int count = selected->get_size();
	unsigned int i;
	if (steam_items_.set_craft_size( count )) {
		for (i = 0; i < count; ++i) {
			Item* item = selected->get_item( i );
			steam_items_.set_craft_item( i, item );
		}
		if (steam_items_.craft_items()) {
			// Block access to inventory until we get a response.
			const JUTIL::ConstantString CRAFT_PENDING( "Waiting for crafting response..." );
			if (!view_->set_loading_notice( &CRAFT_PENDING )) {
				stack->log( "Failed to create craft pending notice." );
				return false;
			}
			pending_deletes_ = true;
			return true;
		}
	}

	// Create an alert here.
	if (!view_->create_alert( &CRAFT_ERROR )) {
		stack->log( "Failed to write craft error message." );
		return false;
	}
	return true;
}

/*
 * Handle item deletion.
 */
bool ItemManager::on_delete_item( void )
{
	JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

	const JUTIL::ConstantString DELETE_ERROR( "Failed to send delete message." );
	DynamicSlotArray* selected = inventory_.get_selected_slots();
	unsigned int count = selected->get_size();
	if (count != 1) {
		stack->log( "Attempting to delete more than one item." );
		return false;
	}
	Item* item = selected->get_item( 0 );
	if (!steam_items_.delete_item( item )) {
		stack->log( "Failed to send item delete message." );
		return false;
	}

	// Create pending message.
	const JUTIL::ConstantString DELETE_PENDING( "Waiting for deletion response..." );
	if (!view_->set_loading_notice( &DELETE_PENDING )) {
		stack->log( "Failed to create delete pending notice." );
		return false;
	}
	pending_deletes_ = true;
	return true;
}

/*
 * Handle selection set change.
 */
void ItemManager::on_selection_changed( void )
{
	view_->update_buttons_state();
}

/*
 * Handle item creation event.
 */
bool ItemManager::on_item_created( Item* item )
{
	// Add item to inventory first.
	if (!inventory_.add_item( item )) {
		return false;
	}

	if (schema_.is_loaded()) {
		// Resolve item definition.
		if (!schema_.resolve( item, &graphics_ )) {
			return false;
		}

		// Place into slot.
		if (!inventory_.place_item( item )) {
			return false;
		}

		// Update excluded slot in case we pushed up.
		view_->update_excluded_page_display();
	}

	return true;
}

/*
 * Handle item deletion event.
 */
void ItemManager::on_item_deleted( uint64 id )
{
	// Find item in inventory.
	Item* item = inventory_.find_item( id );
	if (item != nullptr) {
		inventory_.delete_item( item );

		// If selected items get emptied, re-enable inventory.
		if (pending_deletes_) {
			DynamicSlotArray* selected = inventory_.get_selected_slots();
			if (selected->get_size() == 0) {
				view_->destroy_loading_notice();
				pending_deletes_ = false;
			}
		}

		// Update excluded in case we popped.
		view_->update_excluded_page_display();

		// Update button state since selection may have changed.
		view_->update_buttons_state();
	}
}

/*
 * Handle item update event.
 */
bool ItemManager::on_item_updated( uint64 id, uint32 flags )
{
	// Get error stack.
	JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

	// Find item in inventory.
	Item* item = inventory_.find_item( id );
	if (item == nullptr) {
		stack->log( "Failed to find item to update." );
		return false;
	}
	item->set_inventory_flags( flags );

	// Move to new spot; assume Steam will fix over-riding.
	uint32 index = item->get_position();
	inventory_.displace_item( item );
	if (!inventory_.move_item( item, index )) {
		stack->log( "Failed to place updated item." );
		return false;
	}

	// Update excluded in case we moved out.
	view_->update_excluded_page_display();
	return true;
}

/*
 * Handle craft failure event.
 */
void ItemManager::on_craft_failed( void )
{
	// Get rid of the pending craft notice.
	if (pending_deletes_) {
		view_->destroy_loading_notice();
		pending_deletes_ = false;
	}

	// Post alert.
	const JUTIL::ConstantString CRAFT_ERROR( "Ze item combination. It does nothing! Craft failed!" );
	view_->create_alert( &CRAFT_ERROR );
}

/*
 * Handle inventory size change event.
 */
bool ItemManager::on_inventory_resize( bool is_trial_account, uint32 extra_slots )
{
	// Get error stack.
	JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

    // Calculate number of slots to resize to.
    unsigned int total_slots;
    if (is_trial_account) {
        // Should be no extra slots for trial accounts.
        total_slots = TRIAL_SLOT_COUNT;
    }
    else {
        total_slots = PREMIUM_SLOT_COUNT + extra_slots;
    }
	SlotArray* inventory = inventory_.get_inventory_slots();
    if (total_slots < inventory->get_size()) {
		stack->log( "New backpack is smaller than the old one." );
        return false;
    }

    // Attempt to set inventory book size.
    if (!inventory->set_size( total_slots )) {
        return false;
    }
    return true;
}

/*
 * Handle inventory reset.
 */
void ItemManager::on_inventory_reset( void )
{
	// TODO: tell view to drop selection from here.
	inventory_.delete_items();
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

JUI::IOResult ItemManager::on_mouse_clicked( JUI::Mouse* mouse )
{
	JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

    // Pass message to item manager view.
    JUI::IOResult result = view_->on_mouse_clicked( mouse );
	if (result == JUI::IO_RESULT_ERROR) {
		const JUTIL::String* top = stack->get_top_error();
		if (!view_->create_error( top )) {
			MessageBox( nullptr, top->get_string(), "Error while handling mouse click event!", MB_ICONERROR | MB_OK );
		}
		set_think( &ItemManager::exiting );
	}
    return result;
}

JUI::IOResult ItemManager::on_mouse_released( JUI::Mouse* mouse )
{
	JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

    // Pass message to item manager view.
    JUI::IOResult result = view_->on_mouse_released( mouse );
	if (result == JUI::IO_RESULT_ERROR) {
		const JUTIL::String* top = stack->get_top_error();
		if (!view_->create_error( top )) {
			MessageBox( nullptr, top->get_string(), "Error while handling mouse release event!", MB_ICONERROR | MB_OK );
		}
		set_think( &ItemManager::exiting );
	}
    return result;
}

JUI::IOResult ItemManager::on_mouse_moved( JUI::Mouse* mouse )
{
	JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

	// Pass message to item manager view.
    JUI::IOResult result = view_->on_mouse_moved( mouse );
	if (result == JUI::IO_RESULT_ERROR) {
		const JUTIL::String* top = stack->get_top_error();
		if (!view_->create_error( top )) {
			MessageBox( nullptr, top->get_string(), "Error while handling mouse move event!", MB_ICONERROR | MB_OK );
		}
		set_think( &ItemManager::exiting );
	}
    return result;
}

/*
 * Handle keyboard pressed input.
 */
JUI::IOResult ItemManager::on_key_pressed( int key )
{
	JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

	// Pass message to item manager view.
	JUI::IOResult result = view_->on_key_pressed( key );
	if (result == JUI::IO_RESULT_ERROR) {
		const JUTIL::String* top = stack->get_top_error();
		if (!view_->create_error( top )) {
			MessageBox( nullptr, top->get_string(), "Error while handling key press event!", MB_ICONERROR | MB_OK );
		}
		set_think( &ItemManager::exiting );
	}
	return result;
}

/*
 * Handle keyboard release input.
 */
JUI::IOResult ItemManager::on_key_released( int key )
{
	JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

	// Pass message to item manager view.
	JUI::IOResult result = view_->on_key_released( key );
	if (result == JUI::IO_RESULT_ERROR) {
		const JUTIL::String* top = stack->get_top_error();
		if (!view_->create_error( top )) {
			MessageBox( nullptr, top->get_string(), "Error while handling key press event!", MB_ICONERROR | MB_OK );
		}
		set_think( &ItemManager::exiting );
	}
	return JUI::IO_RESULT_UNHANDLED;
}
