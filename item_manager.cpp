#include <algorithm>
#include <stdlib.h>
#include <crtdbg.h>
#include <fstream>
#include <math.h>
#include <windows.h>
#include <shellapi.h>
#include <json/json.h>

#include <jui/gfx/font_factory.hpp>

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
const char*	APPLICATION_TITLE	= "Jengerer's Item Manager Lite";
const int	APPLICATION_WIDTH	= 795;
const int	APPLICATION_HEIGHT	= 540;
const char*	APPLICATION_VERSION	= "0.9.9.9.7.9";

// UI attributes.
const unsigned int EXIT_BUTTON_PADDING	= 10;
const DWORD PAGE_DELAY_INTERVAL			= 500;

// Title display.
const char* TITLE_FONT_FACE				= "fonts/tf2build.ttf";
const unsigned int TITLE_FONT_SIZE		= 14;
const bool TITLE_FONT_BOLDED			= false;
const JUI::Colour TITLE_COLOUR( 241, 239, 237 );

// Page display.
const char* PAGE_FONT_FACE				= "fonts/tf2build.ttf";
const unsigned int PAGE_FONT_SIZE		= 14;
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

ItemManager::ItemManager( HINSTANCE instance ) : Application( instance )
{
	// Set application size.
	set_size( APPLICATION_WIDTH, APPLICATION_HEIGHT );

	// Set window parameters.
	Window* window = get_window();
	window->set_title( APPLICATION_TITLE );
	window->set_border( true );
	window->set_fullscreen( false );

	// Create the user layer.
	user_layer_ = new Container();
	user_layer_->set_size( get_width(), get_height() );
	add( user_layer_ );

	// Create popup view on top.
	popups_ = new PopupDisplay();
	popups_->set_size( get_width(), get_height() );
	popups_->set_popup_handler( this );
	add( popups_ );

	// Alerts and errors.
	alert_ = nullptr;
	error_ = nullptr;
	update_error_ = false;

	// Item stats.
	item_display_ = nullptr;

	// Slot views.
	inventory_view_ = nullptr;
	excluded_view_ = nullptr;

	// Dragged slot.
	dragged_view_ = nullptr;

	// Create books.
	inventory_book_ = new SlotBook( PAGE_WIDTH, PAGE_HEIGHT );
	inventory_book_->add_pages( DEFAULT_PAGE_COUNT );
	excluded_book_ = new DynamicSlotBook( EXCLUDED_WIDTH, EXCLUDED_HEIGHT );

	// Create backpack of one page.
	backpack_ = new Backpack( inventory_book_, excluded_book_ );

	// Threaded loader.
	definition_loader_ = nullptr;
	load_progress_ = nullptr;

	// Fonts.
	page_font_ = nullptr;
	title_font_ = nullptr;

	// Create Steam interface.
	steam_items_ = new SteamItemHandler();
	page_delay_ = 0;

	// Set default running function.
	set_think( &ItemManager::loading );
}

ItemManager::~ItemManager( void )
{
	close_interfaces();
}

void ItemManager::load_interfaces()
{
	// Get downloader.
	FileDownloader* downloader = FileDownloader::get_instance();
	site_loader_ = new HttpResourceLoader( "http://www.jengerer.com/item_manager/", downloader );

	// Download rounded corner.
	site_loader_->get_resource( "img/manager/rounded_corner.png", "img/manager/rounded_corner.png" );

	// Download button icons.
	site_loader_->get_resource( "img/manager/gear.png", "img/manager/gear.png" );
	site_loader_->get_resource( "img/manager/equip.png", "img/manager/equip.png" );
	site_loader_->get_resource( "img/manager/sort.png", "img/manager/sort.png" );

	// Get placeholder item icon.
	site_loader_->get_resource( "img/backpack/unknown_item.png", "img/backpack/unknown_item.png" );

	// Download fonts.
	site_loader_->get_resource( "fonts/tf2build.ttf", "fonts/tf2build.ttf" );
	site_loader_->get_resource( "fonts/tf2secondary.ttf", "fonts/tf2secondary.ttf" );

	// Start up JUI::Graphics2D* and window.
	Application::load_interfaces();

	// Necessary to display progress/status.
	Notice::precache( graphics_ );
	Button::precache( graphics_ );
	RoundedRectangle::precache( graphics_ );

	try {
		// precache secondary resources.
		ToggleSet::precache();
		ItemDisplay::precache();
		Notification::precache();
		SlotView::precache( graphics_ );

		// Load title font.
		title_font_ = FontFactory::create_font( TITLE_FONT_FACE, TITLE_FONT_SIZE );
		page_font_ = FontFactory::create_font( PAGE_FONT_FACE, PAGE_FONT_SIZE );

		// Create notification queue.
		notifications_ = new NotificationQueue();
		notifications_->set_position( 
			static_cast<float>(get_width() - PADDING), 
			static_cast<float>(get_height() - PADDING) );
		add( notifications_ );

		// Generate UI.
		create_layout();

		// Check for latest version.
		if (!is_latest_version()) {
			update_error_ = true;
			throw std::runtime_error( "A new version of the item manager is out and needs to be downloaded against your will. Press okay to continue." );
		}

		// Ensure TF2's not running.
		if (get_process_count( "tf2.exe" ) > 0) {
			throw std::runtime_error( "Please close Team Fortress 2 before running the item manager." );
		}
		else if (get_process_count( "item_manager.exe" ) > 1) {
			throw std::runtime_error( "Another instance of the item manager is already running!" );
		}

		// Start definition loader.
		load_progress_ = popups_->create_notice( "Preparing to load item definitions..." );
		load_definitions();
	}
	catch (std::runtime_error& load_ex) {
		error_ = popups_->create_alert( load_ex.what() );
		set_think( &ItemManager::exiting );
	}
}

void ItemManager::close_interfaces( void )
{
	if (title_font_ != nullptr) {
		delete title_font_;
		title_font_ = nullptr;
	}

	if (page_font_ != nullptr) {
		delete page_font_;
		page_font_ = nullptr;
	}

	if (mouse_ != nullptr) {
		delete mouse_;
		mouse_ = nullptr;
	}

	if (steam_items_ != nullptr) {
		delete steam_items_;
		steam_items_ = nullptr;
	}

	if (backpack_ != nullptr) {
		delete backpack_;
		backpack_ = nullptr;
	}

	if (inventory_book_ != nullptr) {
		delete inventory_book_;
		inventory_book_ = nullptr;
	}

	if (excluded_book_ != nullptr) {
		delete excluded_book_;
		excluded_book_ = nullptr;
	}

	if (definition_loader_ != nullptr) {
		delete definition_loader_;
		definition_loader_ = nullptr;
	}

	// Erase item definitions.
	for (auto i = Item::definitions.begin(); i != Item::definitions.end(); i = Item::definitions.erase( i )) {
		delete i->second;
	}

	// Erase attribute definitions.
	for (auto i = Item::attributes.begin(); i != Item::attributes.end(); i = Item::attributes.erase( i )) {
		delete i->second;
	}

	// Erase fallback definition.
	if (Item::fallback != nullptr) {
		delete Item::fallback;
		Item::fallback = nullptr;
	}

	// Delete site loader.
	if (site_loader_ != nullptr) {
		delete site_loader_;
		site_loader_ = nullptr;
	}

	// Free cached resources.
	ItemDisplay::release();
	Button::release();
	Notice::release();
	Notification::release();
	SlotView::release();
	ToggleSet::release();

	// Close font library.
	FontFactory::shut_down();

	// Free all protobuf library resources.
	google::protobuf::ShutdownProtobufLibrary();

	// Close downloader.
	FileDownloader::shut_down();
}

void ItemManager::create_layout( void )
{
	// Create layout.
	JUI::VerticalLayout* layout = new JUI::VerticalLayout( SPACING, ALIGN_LEFT );

	// Create inventory view.
	inventory_view_ = new AnimatedBookView( inventory_book_,
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
}

void ItemManager::run( void )
{
	try {
		Application::run();
		think();
		draw_frame();
	}
	catch (const std::exception& ex) {
		error_ = popups_->create_alert( ex.what() );
		set_think( &ItemManager::exiting );
	}
}

void ItemManager::set_think( void (ItemManager::*thinkFunction)( void ) )
{
	think_function_ = thinkFunction;
}

void ItemManager::think()
{
	(this->*think_function_)();
}

void ItemManager::loading()
{
	if (definition_loader_ != nullptr) {
		switch (definition_loader_->get_state()){
		case LOADING_STATE_ERROR:
			// Show error first.
			error_ = popups_->create_alert( definition_loader_->get_progress_msg() );

			// Remove threaded loader.
			delete definition_loader_;
			definition_loader_ = nullptr;
			set_think( &ItemManager::exiting );
			break;

		case LOADING_STATE_FINISHED:
			// Remove threaded loader.
			delete definition_loader_;
			definition_loader_ = nullptr;

			// Initialize steam.
			try {
				steam_items_->load_interfaces();
			}
			catch (const std::runtime_error& ex) {
				error_ = popups_->create_alert( ex.what() );
				set_think( &ItemManager::exiting );
				break;
			}

			load_progress_->set_message( "Waiting for Steam message..." );
			load_progress_->center_to( this );
			break;

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

void ItemManager::running()
{
	handle_callback();
	notifications_->update_notifications();
	inventory_view_->update_view();
	update_item_display();
}

void ItemManager::exiting()
{
	// Just wait for exit.
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
		if (steam_items_->get_select_mode() != SELECT_MODE_MULTIPLE) {
			steam_items_->deselect_all();
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
				if (steam_items_->is_selected_tradable()) {
					steam_items_->craft_selected();
				}
				else {
					craft_check_ = popups_->create_confirmation( "One or more of the items you've selected are not tradable. The result will not be tradable. Continue?" );
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
		switch (steam_items_->get_select_mode()) {
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
				steam_items_->deselect_all();

				// Start dragging.
				float view_x = slot_view->get_x();
				float view_y = slot_view->get_y();
				dragged_view_ = new DraggedSlotView( slot );
				dragged_view_->set_position( view_x, view_y );
				dragged_view_->set_offset( view_x - mouse->get_x(), view_y - mouse->get_y() );
				dragged_view_->set_alpha( 200 );
				steam_items_->select( dragged_view_ );
				add( dragged_view_ );
				break;
			}

		case SELECT_MODE_MULTIPLE:
			steam_items_->toggle_select( slot_view );
			break;
		}
	}
	else if (steam_items_->get_select_mode() != SELECT_MODE_MULTIPLE) {
		steam_items_->deselect_all();
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
				steam_items_->update_item( dragged_item );
				steam_items_->update_item( touched_item );
			}
		}
		else {
			dragged_slot->remove_item();
			backpack_->move_item( dragged_item, touched_slot->get_index() );
			steam_items_->update_item( dragged_item );
		}
	}

	update_buttons();
}

void ItemManager::update_buttons()
{
	unsigned int selectedCount = steam_items_->get_selected_count();
	craft_button_->set_enabled( selectedCount != 0 );

#ifndef EQUIP_NOT_IMPLEMENTED
	equip_button_->set_enabled( steam_items_->can_equip_selected() );
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
		steam_items_->set_select_mode( SELECT_MODE_MULTIPLE );
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
		steam_items_->set_select_mode( SELECT_MODE_SINGLE );
		break;
	}

	return true;
}

void ItemManager::update_item_display( void )
{
	item_display_->update_alpha();
}

void ItemManager::update_page_display( void )
{
	// Update text.
	stringstream page_text;
	page_text << (inventory_view_->get_active_page() + 1) << '/' << inventory_view_->get_page_count();
	page_display_->set_text( page_text.str() );

	// Update buttons.
	unsigned int page_index = inventory_view_->get_active_page();
	unsigned int last_index = inventory_view_->get_page_count() - 1;
	prev_button_->set_enabled( page_index != 0 );
	next_button_->set_enabled( page_index != last_index );
}

void ItemManager::load_definitions( void )
{
	// Set the message and redraw.
	load_progress_->set_message("Loading item definitions...");

	// Set up loader.
	definition_loader_ = new DefinitionLoader( graphics_ );
	definition_loader_->begin();
}

void ItemManager::load_items_from_web( void )
{
	load_progress_->append_message("\n\nLoading items...");
	draw_frame();

	uint64 userId = steam_items_->get_steam_id();
	stringstream urlStream;
	urlStream << "http://api.steampowered.com/ITFItems_440/GetPlayerItems/v0001/?key=0270F315C25E569307FEBDB67A497A2E&SteamID=" << userId << "&format=json";
	string apiUrl = urlStream.str();

	// Attempt to read the file.
	string jsonInventory;
	try {
		FileDownloader* downloader = FileDownloader::get_instance();
		jsonInventory = downloader->read( apiUrl );
	}
	catch (std::runtime_error curl_ex) {
		throw std::runtime_error( "Failed to read inventory from profile." );
	}

	// Parse file.
	Json::Value root;
	Json::Reader reader;
	if (!reader.parse( jsonInventory, root, false )) {
		throw new std::runtime_error( "Failed to parse inventory JSON file." );
	}

	// Get result.
	const Json::Value& result = root["result"];
	
	// Check status.
	int status = result["status"].asInt();
	if (status != 1) {
		throw new std::runtime_error( "Failed to get player inventory." );
	}

	// Add more slots.
	int slots = result["num_backpack_slots"].asInt();
	unsigned int current_slots = inventory_book_->get_slot_count();
	if (current_slots > slots) {
		throw std::runtime_error( "Web API reports fewer slots than in current backpack." );
	}
	
	unsigned int added_slots = slots - current_slots;
	unsigned int added_pages = added_slots / inventory_book_->get_page_size();
	inventory_book_->add_pages( added_pages );
	inventory_view_->update_pages();
	update_page_display();

	// Get items.
	Json::Value items = result["item"];
	for (Json::ValueIterator i = items.begin(); i != items.end(); ++i) {
		const Json::Value& item = *i;
		
		Item* new_item = new Item(
			item["id"].asUInt64(),
			item["defindex"].asUInt(),
			item["level"].asUInt(),
			(EItemQuality)item["quality"].asUInt(),
			item["quantity"].asUInt(),
			item["inventory"].asUInt(),
			0 );
		backpack_->insert_item( new_item );
	}

	// Show success.
	load_progress_->set_message("Items successfully loaded!");
	draw_frame();

	backpack_->set_loaded( true );
}

bool ItemManager::is_latest_version() const
{
	// Check for program updates.
	try {
		FileDownloader* downloader = FileDownloader::get_instance();
		string version = downloader->read( "http://www.jengerer.com/item_manager/item_manager.txt" );
		return version == APPLICATION_VERSION;
	}
	catch (const std::runtime_error&) {
		// Failed to get version, it's our fault!
	}

	return true;
}

void ItemManager::launch_updater() const
{
	// Get updater if not exists.
	site_loader_->get_resource( "auto_updater.exe", "auto_updater.exe" );

	// TODO: Make an error type enum and launch this on update error.
	int result = (int)ShellExecute( 0, 0, "auto_updater.exe", 0, 0, SW_SHOWDEFAULT );
	if (result <= 32) {
		// No exception, just warn to manually update.
		MessageBox( nullptr, "Failed to run updater, try re-downloading the application if this persists.", "Automatic Update Failed", MB_ICONEXCLAMATION | MB_OK );
	}
}

void ItemManager::handle_callback( void ) {
	CallbackMsg_t callback;
	if ( steam_items_->get_callback( &callback ) ) {
		switch (callback.m_iCallback) {
		case GCMessageAvailable_t::k_iCallback:
			{
				GCMessageAvailable_t *message = (GCMessageAvailable_t *)callback.m_pubParam;
				
				uint32 size;
				if ( steam_items_->has_message( &size ) )
				{
					uint32 id, realSize = 0;
					BYTE* buffer = new BYTE[size];

					try {
						steam_items_->get_message( &id, buffer, size, &realSize );

						// Filter protobuf messages.
						if ((id & 0x80000000) != 0) {
							uint32 realId = id & 0x0FFFFFFF;

							// First get the protobuf struct header.
							SerializedBuffer headerBuffer(buffer);
							GCProtobufHeader_t *headerStruct = headerBuffer.get<GCProtobufHeader_t>();
							uint32 headerSize = headerStruct->m_cubProtobufHeader;

	#ifdef _DEBUG
							stringstream protoMsg;
							protoMsg << "Protobuf message of type " << realId << " received.";
							notifications_->add_notification( protoMsg.str(), nullptr );
	#endif

							// Now get the real protobuf header.
							CMsgProtoBufHeader headerMsg;
							void *headerBytes = headerBuffer.here();
							headerMsg.ParseFromArray( headerBytes, headerSize );
							headerBuffer.push( headerSize );

							// Check if we can set target ID.
							// TODO: Maybe move all this horseshit into Steam.
							if ( headerMsg.has_job_id_source() ) {
								steam_items_->set_target_id( headerMsg.job_id_source() );
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

		steam_items_->release_callback();
	} 
}

void ItemManager::handle_message( uint32 id, void* message, size_t size )
{
	switch (id) {
		case GCCraftResponse_t::k_iMessage:
		{
			GCCraftResponse_t *craftMsg = static_cast<GCCraftResponse_t*>(message);
			if (craftMsg->blueprint == 0xffff) {
				notifications_->add_notification( "Crafting failed. No such blueprint!", nullptr );
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

			steam_items_->set_version( cache_msg.version() );

			// Check that this is our backpack.
			if (cache_msg.owner() != steam_items_->get_steam_id()) {
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
						if (steam_items_->get_steam_id() == cache_msg.owner()) {
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
			if (steam_items_->get_version() != version) {
				steam_items_->set_version( version );

				// Send refresh.
				CMsgSOCacheSubscriptionRefresh refresh;
				refresh.set_owner( steam_items_->get_steam_id() );
				string refreshString = refresh.SerializeAsString();
				steam_items_->send_message( 28 | 0x80000000, (void*)refreshString.c_str(), refreshString.size() );
			}
			break;
		}

	case k_ESOMsg_Update:
		{
			CMsgSOSingleObject update_msg;
			update_msg.ParseFromArray( message, size );
			steam_items_->set_version( update_msg.version() );

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
			steam_items_->set_version( update_msg.version() );

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
			steam_items_->set_version( create_msg.version() );

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
			steam_items_->set_version( delete_msg.version() );

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
				steam_items_->craft_selected();
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