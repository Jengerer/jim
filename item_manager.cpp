#include "item_manager.h"

#include <algorithm>
#include <stdlib.h>
#include <crtdbg.h>
#include <fstream>
#include <math.h>
#include <shellapi.h>
#include <json/json.h>

#include "curl.h"
#include "font_factory.h"
#include "serialized_buffer.h"
#include "slot_view.h"

#include "protobuf/base_gcmessages.pb.h"
#include "protobuf/steammessages.pb.h"
#include "protobuf/gcsdk_gcmessages.pb.h"

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
const char*	APPLICATION_VERSION	= "0.9.9.9.7.4";

// UI attributes.
const unsigned int EXIT_BUTTON_PADDING	= 10;
const DWORD PAGE_DELAY_INTERVAL			= 500;

// Title display.
const char* TITLE_FONT_FACE				= "fonts/tf2build.ttf";
const unsigned int TITLE_FONT_SIZE		= 14;
const bool TITLE_FONT_BOLDED			= false;
const Colour TITLE_COLOUR( 241, 239, 237 );

// Page display.
const char* PAGE_FONT_FACE				= "fonts/tf2build.ttf";
const unsigned int PAGE_FONT_SIZE		= 14;
const bool PAGE_FONT_BOLDED				= false;
const Colour PAGE_LABEL_COLOUR( 201, 79, 57 );
const unsigned int PAGE_LABEL_WIDTH		= 50;

// Item display attributes.
const unsigned int ITEM_DISPLAY_SPACING	= 10;

// Frame rate limiters.
const DWORD FRAME_SPEED = 1000 / 60;

// Inventory attributes.
const int PAGE_WIDTH		= 10;
const int PAGE_HEIGHT		= 5;
const int PAGE_COUNT		= 6;
const int TRIAL_PAGE_COUNT	= 1;
const int EXCLUDED_SIZE		= 5;

// Slot layout.
const int SLOT_SPACING			= 5;
const int PAGE_SPACING			= 50;

const unsigned int PADDING	= 20;
const unsigned int SPACING	= 10;

LRESULT CALLBACK wndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message) {
	case WM_CLOSE:
	case WM_DESTROY:
		PostQuitMessage( 0 );
		return 0;
		break;
	}

	return DefWindowProc( hWnd, message, wParam, lParam );
}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{

#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
#endif

	ItemManager* itemManager = new ItemManager();

	try {
		itemManager->load_interfaces( hInstance );
	}
	catch (std::runtime_error ex) {
		MessageBox( NULL, ex.what(), "Initialization failed!", MB_OK );
	}

	// Enter main program loop.
	MSG msg;
	bool running = true;
	while (running) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				running = false;
			}
			else {
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
		}

		itemManager->run();
	}

	// Clean item manager.
	if (itemManager != nullptr) {
		delete itemManager;
		itemManager = nullptr;
	}

#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
#endif

	return EXIT_SUCCESS;
}

ItemManager::ItemManager( void ) : Application( APPLICATION_WIDTH, APPLICATION_HEIGHT )
{
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
	updateError_ = false;

	// Item stats.
	itemDisplay_ = nullptr;

	// Slot views.
	inventoryView_ = nullptr;
	excludedView_ = nullptr;

	// Dragged slot.
	dragTarget_ = nullptr;
	draggedView_ = nullptr;

	// Create backpack of one page.
	backpack_ = new Backpack( EXCLUDED_SIZE );
	backpack_->add_slots( PAGE_WIDTH * PAGE_HEIGHT );

	// Threaded loader.
	definitionLoader_ = nullptr;
	loadProgress_ = nullptr;

	// Fonts.
	pageFont_ = nullptr;
	titleFont_ = nullptr;

	// Create Steam interface.
	steamItems_ = new SteamItemHandler();
	pageDelay_ = 0;

	// Set default running function.
	set_think( &ItemManager::loading );

	// Listen for input keys.
	AddKey( VK_LEFT );
	AddKey( VK_RIGHT );
	AddKey( VK_ESCAPE );
	AddKey( VK_RETURN );
	AddKey( VK_CONTROL );
}

ItemManager::~ItemManager( void )
{
	close_interfaces();
}

void ItemManager::load_interfaces( HINSTANCE instance )
{
	// Start up Graphics2D and window.
	Application::load_interfaces( APPLICATION_TITLE, instance );

	// Necessary to display progress/status.
	FontFactory::initialize();
	Notice::precache( graphics_ );
	Button::precache( graphics_ );

	try {
		// Check for latest version.
		if (!is_latest_version()) {
			updateError_ = true;
			throw std::runtime_error( "A new version of the item manager is out and needs to be downloaded against your will. Press okay to continue." );
		}

		// Ensure TF2's not running.
		if (get_process_count( "tf2.exe" ) > 0) {
			throw std::runtime_error( "Please close Team Fortress 2 before running the item manager." );
		}
		else if (get_process_count( "item_manager.exe" ) > 1) {
			throw std::runtime_error( "Another instance of the item manager is already running!" );
		}

		// precache secondary resources.
		ToggleSet::precache();
		ItemDisplay::precache();
		Notification::precache();
		SlotView::precache( graphics_ );

		// Load title font.
		titleFont_ = FontFactory::create_font( TITLE_FONT_FACE, TITLE_FONT_SIZE );
		pageFont_ = FontFactory::create_font( PAGE_FONT_FACE, PAGE_FONT_SIZE );

		// Create notification queue.
		notifications_ = new NotificationQueue();
		notifications_->set_position( 
			static_cast<float>(get_width() - PADDING), 
			static_cast<float>(get_height() - PADDING) );
		add( notifications_ );

		// Generate UI.
		create_layout();

		// Start definition loader.
		loadProgress_ = popups_->create_notice( "Preparing to load item definitions..." );
		load_definitions();
	}
	catch (std::runtime_error& load_ex) {
		error_ = popups_->create_alert( load_ex.what() );
		set_think( &ItemManager::exiting );
	}
}

void ItemManager::close_interfaces( void )
{
	if (titleFont_ != nullptr) {
		delete titleFont_;
		titleFont_ = nullptr;
	}

	if (pageFont_ != nullptr) {
		delete pageFont_;
		pageFont_ = nullptr;
	}

	if (mouse_ != nullptr) {
		delete mouse_;
		mouse_ = nullptr;
	}

	if (steamItems_ != nullptr) {
		delete steamItems_;
		steamItems_ = nullptr;
	}

	if (backpack_ != nullptr) {
		delete backpack_;
		backpack_ = nullptr;
	}

	if (definitionLoader_ != nullptr) {
		delete definitionLoader_;
		definitionLoader_ = nullptr;
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

	// Free cached resources.
	ItemDisplay::Release();
	Button::Release();
	Notice::Release();
	Notification::Release();
	SlotView::Release();
	ToggleSet::Release();

	// Close font library.
	FontFactory::shut_down();
	
	// Close curl.
	Curl::shut_down();

	// Free all protobuf library resources.
	google::protobuf::ShutdownProtobufLibrary();
}

void ItemManager::create_layout( void )
{
	// Create layout.
	VerticalLayout* layout = new VerticalLayout( SPACING, ALIGN_LEFT );

	// Create inventory view.
	inventoryView_ = new AnimatedGridPages( PAGE_WIDTH, PAGE_HEIGHT,
		PAGE_SPACING, SLOT_SPACING );
	inventoryView_->add_pages( backpack_->get_inventory_slots() );

	// Create excluded view.
	excludedView_ = new SlotGridView( EXCLUDED_SIZE, SLOT_SPACING );
	excludedView_->add_slots( backpack_->get_excluded_slots() );

	// Create button layout.
	Texture *craftTexture = graphics_->get_texture( "manager/gear" );
	Texture *equipTexture = graphics_->get_texture( "manager/equip" );
	Texture *sortTexture = graphics_->get_texture( "manager/sort" );

	// Create inventory buttons.
	craftButton_ = Button::CreateIconLabelButton( craftTexture, "craft" );
	equipButton_ = Button::CreateIconLabelButton( equipTexture, "equip" );
	sortButton_ = Button::CreateIconLabelButton( sortTexture, "sort" );
	craftButton_->SetEnabled( false );
	equipButton_->SetEnabled( false );
	sortButton_->SetEnabled( false );
	
	// Create inventory button layout.
	HorizontalLayout* inventoryButtons = new HorizontalLayout( BUTTON_SPACING );
	inventoryButtons->add( craftButton_ );
	inventoryButtons->add( equipButton_ );
	inventoryButtons->add( sortButton_ );
	inventoryButtons->pack();

	// Create pages buttons/text.
	prevButton_ = Button::CreateLabelButton( "<" );
	nextButton_ = Button::CreateLabelButton( ">" );
	pageDisplay_ = new WrappedText( pageFont_, PAGE_LABEL_WIDTH );
	pageDisplay_->SetColour( PAGE_LABEL_COLOUR );
	pageDisplay_->set_text_formatting( DT_CENTER );
	update_page_display();

	// Create pages buttons layout.
	HorizontalLayout* pageButtons = new HorizontalLayout( BUTTON_SPACING );
	pageButtons->add( prevButton_ );
	pageButtons->add( pageDisplay_ ); // PAGE LABEL, RATHER
	pageButtons->add( nextButton_ );
	pageButtons->pack();

	HorizontalSplitLayout* buttonLayout = new HorizontalSplitLayout( inventoryView_->get_width() );
	buttonLayout->add_left( inventoryButtons);
	buttonLayout->add_right( pageButtons );
	buttonLayout->pack();
		
	// Create title.
	stringstream titleStream;
	titleStream << APPLICATION_TITLE << " " << APPLICATION_VERSION;

	// Add version number.
	Text* titleText = new Text( titleFont_ );
	titleText->SetText( titleStream.str() );
	titleText->SetColour( TITLE_COLOUR );

	// Organize layout.
	layout->add( titleText );
	layout->add( inventoryView_ );
	layout->add( buttonLayout );
	layout->add( excludedView_ );
	layout->pack();
	layout->set_position( (get_width() - layout->get_width()) / 2.0f, 
		(get_height() - layout->get_height()) / 2.0f );
	user_layer_->add( layout );

	// Create item display.
	itemDisplay_ = new ItemDisplay();
	user_layer_->add( itemDisplay_ );
}

void ItemManager::run( void )
{
	Application::run();
	think();
	draw_frame();
}

void ItemManager::set_think( void (ItemManager::*thinkFunction)( void ) )
{
	thinkFunction_ = thinkFunction;
}

void ItemManager::think()
{
	(this->*thinkFunction_)();
}

void ItemManager::loading()
{
	if (definitionLoader_ != nullptr) {
		switch (definitionLoader_->get_state()){
		case LOADING_STATE_ERROR:
			// Show error first.
			error_ = popups_->create_alert( definitionLoader_->get_progress_msg() );

			// Remove threaded loader.
			delete definitionLoader_;
			definitionLoader_ = nullptr;
			set_think( &ItemManager::exiting );
			break;

		case LOADING_STATE_FINISHED:
			// Remove threaded loader.
			delete definitionLoader_;
			definitionLoader_ = nullptr;

			// Initialize steam.
			try {
				steamItems_->load_interfaces();
			}
			catch (const std::runtime_error& ex) {
				error_ = popups_->create_alert( ex.what() );
				set_think( &ItemManager::exiting );
				break;
			}

			loadProgress_->SetMessage( "Waiting for Steam message..." );
			loadProgress_->center_to( this );
			break;

		default:
			definitionLoader_->update_progress_msg();
			loadProgress_->SetMessage( definitionLoader_->get_progress_msg() );
			loadProgress_->center_to( this );
			break;
		}
	}
	else {
		handle_callback();
		if (backpack_ != nullptr && backpack_->is_loaded()) {
			set_think( &ItemManager::running );
			popups_->remove_popup( loadProgress_ );
#ifndef SORT_NOT_IMPLEMENTED
			sortButton_->SetEnabled( true );
#endif
		}
	}
}

void ItemManager::running()
{
	handle_callback();
	notifications_->UpdateNotifications();
	inventoryView_->update_view();
	update_item_display();
}

void ItemManager::exiting()
{
	// Just wait for exit.
}

bool ItemManager::on_mouse_clicked( Mouse *mouse )
{
	// Mouse clicked.
	if (popups_->on_mouse_clicked( mouse )) {
		return true;
	}
	else {
		if (draggedView_ != nullptr) {
			return true;
		}

		SlotView* slotView = nullptr;
		bool touchedView = true;

		// Attempt to get a slot.
		if (mouse->is_touching( inventoryView_ )) {
			slotView = inventoryView_->get_touching_slot( mouse );
		}
		else if (mouse->is_touching( excludedView_ )) {
			slotView = excludedView_->get_touching_slot( mouse );
		}
		else {
			touchedView = false;
		}

		if (touchedView) {
			if (slotView != nullptr) {
				on_slot_clicked( slotView, mouse );
				return true;
			}
		}
		else if (!touchedView) {
			// TODO: Set a 'clicked target' pointer so we can't just release on button and trigger.
			if (mouse->is_touching( craftButton_ ) ||
				mouse->is_touching( equipButton_ ) ||
				mouse->is_touching( sortButton_ ) ||
				mouse->is_touching( nextButton_ ) ||
				mouse->is_touching( prevButton_ )) {
					return true;
			}
			else if (notifications_->on_mouse_clicked( mouse )) {
				return true;
			}
		}

		// Deselect all, nothing clicked.
		if (steamItems_->get_select_mode() != SELECT_MODE_MULTIPLE) {
			steamItems_->deselect_all();
			update_buttons();
		}
	}

	return false;
}

bool ItemManager::on_mouse_released( Mouse *mouse )
{
	// Check top popup.
	if (popups_->on_mouse_released( mouse )) {
		return true;
	}
	else {
		// Release slot if dragging.
		if (draggedView_ != nullptr) {
			SlotView* slotView = inventoryView_->get_touching_slot( mouse );
			on_slot_released( slotView );
		}
		else {
			// Handle buttons.
			if (craftButton_->on_mouse_released( mouse )) {
				if (steamItems_->is_selected_tradable()) {
					steamItems_->craft_selected();
				}
				else {
					craft_check_ = popups_->create_confirmation( "One or more of the items you've selected are not tradable. The result will not be tradable. Continue?" );
				}
			}
			else if (nextButton_->on_mouse_released( mouse )) {
				inventoryView_->next_page();
				update_page_display();
			}
			else if (prevButton_->on_mouse_released( mouse )) {
				inventoryView_->previous_page();
				update_page_display();
			}
		}
	}

	return false;
}

bool ItemManager::on_mouse_moved( Mouse *mouse )
{
	// Update buttons.
	// TODO: Have a button frame mouse hover state.
	itemDisplay_->set_item( nullptr );

	// Move over all buttons.
	craftButton_->on_mouse_moved( mouse );
	equipButton_->on_mouse_moved( mouse );
	sortButton_->on_mouse_moved( mouse );
	nextButton_->on_mouse_moved( mouse );
	prevButton_->on_mouse_moved( mouse );

	// Pass to highest popup.
	if (popups_->on_mouse_moved( mouse )) {
		return true;
	}
	else {
		// Check if dragging.
		if (draggedView_ != nullptr) {
			draggedView_->on_mouse_moved( mouse );
			clamp_child( draggedView_ );

			// Check if we're switching page.
			DWORD currentTick = GetTickCount();
			if (currentTick > pageDelay_) {
				if (draggedView_->get_x() <= 0.0f) {
					if (inventoryView_->previous_page()) {
						pageDelay_ = currentTick + PAGE_DELAY_INTERVAL;
					}

					update_page_display();
				}
				else if (draggedView_->get_x() + draggedView_->get_width() >= get_width()) {
					if (inventoryView_->next_page()) {
						pageDelay_ = currentTick + PAGE_DELAY_INTERVAL;
					}

					update_page_display();
				}
			}	
		}
		else {
			// Check if we've hovering over an slot view.
			SlotView* slotView = nullptr;
			if (mouse->is_touching( inventoryView_ )) {
				slotView = inventoryView_->get_touching_slot( mouse );
			}
			else if (mouse->is_touching( excludedView_ )) {
				slotView = excludedView_->get_touching_slot( mouse );
			}
			else {
				return false;
			}
			
			// Did we get one?
			if (slotView != nullptr) {
				Slot* slot = slotView->get_slot();
				// Only display for full slots.
				if (slot->has_item()) {
					itemDisplay_->set_item( slot->get_item() );
					float displayX = slotView->get_x() + (slotView->get_width() - itemDisplay_->get_width()) / 2.0f;
					float displayY = slotView->get_y() + slotView->get_height() + ITEM_DISPLAY_SPACING;
					if (displayY + itemDisplay_->get_height() > get_height()) {
						displayY = slotView->get_y() - itemDisplay_->get_height() - ITEM_DISPLAY_SPACING;
					}

					itemDisplay_->set_position(
						static_cast<float>(displayX),
						static_cast<float>(displayY) );
					clamp_child( itemDisplay_, static_cast<float>(PADDING) );
				}
			}
		}

		return true;
	}

	return false;
}

void ItemManager::on_slot_clicked( SlotView* slotView, Mouse* mouse )
{
	Slot* slot = slotView->get_slot();
	if (slot->has_item()) {
		switch (steamItems_->get_select_mode()) {
		case SELECT_MODE_SINGLE:
			{
				assert( draggedView_ == nullptr );
				Item* item = slot->get_item();
				steamItems_->deselect_all();

				// Start dragging.
				dragTarget_ = slotView;
				float viewX = slotView->get_x();
				float viewY = slotView->get_y();
				draggedView_ = new DraggedSlotView( new Slot );
				draggedView_->set_position( viewX, viewY );
				draggedView_->SetOffset( viewX - mouse->get_x(), viewY - mouse->get_y() );
				draggedView_->set_alpha( 200 );
				steamItems_->select( draggedView_ );

				// Swap items.
				draggedView_->get_slot()->set_item( item );
				slot->set_item( nullptr );

				add( draggedView_ );
				break;
			}

		case SELECT_MODE_MULTIPLE:
			steamItems_->toggle_select( slotView );
			break;
		}
	}
	else if (steamItems_->get_select_mode() != SELECT_MODE_MULTIPLE) {
		steamItems_->deselect_all();
	}

	update_buttons();
}

void ItemManager::on_slot_released( SlotView* slotView )
{
	Slot* draggedSlot = draggedView_->get_slot();
	Item* draggedItem = draggedSlot->get_item();
	backpack_->remove_item( draggedItem );
	steamItems_->deselect_all();

	// Move back if not excluded.
	bool isExcluded = !backpack_->CanInsert( draggedItem );
	Slot* targetSlot = dragTarget_->get_slot();

	// Delete temporary dragged.
	SlotView* selectTarget = dragTarget_;
	remove( draggedView_ );
	delete draggedView_;
	draggedView_ = nullptr;
	dragTarget_ = nullptr;

	bool toInventory = false;
	if (slotView != nullptr) {
		// Now try move slot.
		Slot* touchedSlot = slotView->get_slot();
		if (touchedSlot->has_item()) {
			Item* touchedItem = touchedSlot->get_item();
			if (!isExcluded && targetSlot != touchedSlot) {
				// Swap slots.
				targetSlot->set_item( touchedItem );
				touchedSlot->set_item( draggedItem );
				draggedItem->set_position( draggedItem->get_index() );
				touchedItem->set_position( touchedItem->get_index() );
				steamItems_->update_item( touchedItem );
				steamItems_->update_item( draggedItem );
				selectTarget = slotView;
				toInventory = true;
			}
		}
		else {
			targetSlot->set_item( nullptr );
			touchedSlot->set_item( draggedItem );
			draggedItem->set_position( draggedItem->get_index() );
			steamItems_->update_item( draggedItem );
			selectTarget = slotView;
			toInventory = true;
		}
	}
	else if (!isExcluded) {
		targetSlot->set_item( draggedItem );
		toInventory = true;
	}

	// Move item to appropriate group.
	if (toInventory) {
		backpack_->ToInventory( draggedItem );
	}
	else {
		backpack_->ToExcluded( draggedItem );
	}

	steamItems_->select( selectTarget );
	backpack_->update_excluded();
	update_buttons();
}

void ItemManager::update_buttons()
{
	unsigned int selectedCount = steamItems_->get_selected_count();
	craftButton_->SetEnabled( selectedCount != 0 );

#ifndef EQUIP_NOT_IMPLEMENTED
	equipButton_->SetEnabled( steamItems_->can_equip_selected() );
#endif
}

void ItemManager::handle_keyboard( void )
{
	if (IsKeyPressed( VK_ESCAPE )) {
		exit_application();
	}
	else {
		if (IsKeyClicked( VK_RETURN )) {
			popups_->on_key_pressed( VK_RETURN );
		}
		else {
			if (IsKeyClicked( VK_CONTROL )) {
				steamItems_->set_select_mode( SELECT_MODE_MULTIPLE );
			}
			else if (this->IsKeyReleased( VK_CONTROL )) {
				steamItems_->set_select_mode( SELECT_MODE_SINGLE );
			}
	
			if (IsKeyClicked( VK_RIGHT )) {
				inventoryView_->next_page();
				update_page_display();
			}
			else if (IsKeyClicked( VK_LEFT )) {
				inventoryView_->previous_page();
				update_page_display();
			}
		}		
	}
}

void ItemManager::update_item_display( void )
{
	itemDisplay_->UpdateAlpha();
}

void ItemManager::update_page_display( void )
{
	// Update text.
	stringstream pageText;
	pageText << (inventoryView_->get_page_index() + 1) << '/' << inventoryView_->get_page_count();
	pageDisplay_->SetText( pageText.str() );

	// Update buttons.
	unsigned int pageIndex = inventoryView_->get_page_index();
	unsigned int lastIndex = inventoryView_->get_page_count() - 1;
	prevButton_->SetEnabled( pageIndex != 0 );
	nextButton_->SetEnabled( pageIndex != lastIndex );
}

void ItemManager::load_definitions( void )
{
	// Set the message and redraw.
	loadProgress_->SetMessage("Loading item definitions...");

	// Set up loader.
	definitionLoader_ = new DefinitionLoader( graphics_ );
	definitionLoader_->begin();
}

void ItemManager::load_items_from_web( void )
{
	loadProgress_->AppendMessage("\n\nLoading items...");
	draw_frame();

	uint64 userId = steamItems_->get_steam_id();
	stringstream urlStream;
	urlStream << "http://api.steampowered.com/ITFItems_440/GetPlayerItems/v0001/?key=0270F315C25E569307FEBDB67A497A2E&SteamID=" << userId << "&format=json";
	string apiUrl = urlStream.str();

	// Attempt to read the file.
	string jsonInventory;
	try {
		Curl* curl = Curl::get_instance();
		jsonInventory = curl->read( apiUrl );
	}
	catch (std::runtime_error curl_ex) {
		throw std::runtime_error( "Failed to read inventory from profile." );
	}

	// Show success.
	loadProgress_->SetMessage("Items successfully loaded!");
	draw_frame();

	backpack_->set_loaded( true );
}

bool ItemManager::is_latest_version() const
{
	// Check for program updates.
	try {
		Curl* curl = Curl::get_instance();
		string versionInfo = curl->read( "http://www.jengerer.com/item_manager/item_manager.txt" );
		return versionInfo == APPLICATION_VERSION;
	}
	catch (const std::runtime_error&) {
		// Failed to get version, allow it.
	}

	return true;
}

void ItemManager::launch_updater() const
{
	// TODO: Make an error type enum and launch this on update error.
	int result = (int)ShellExecute( 0, 0, "auto_updater.exe", 0, 0, SW_SHOWDEFAULT );
	if (result == ERROR_FILE_NOT_FOUND || result == ERROR_PATH_NOT_FOUND) {
		// Attempt to download the updater.
		try {
			Curl* curl = Curl::get_instance();
			curl->download( "http://www.jengerer.com/item_manager/auto_updater.exe", "auto_updater.exe" );

			// ShellExecute returns >32 if success.
			int result = (int)ShellExecute( 0, 0, "auto_updater.exe", 0, 0, SW_SHOWDEFAULT );
			if (result <= 32) {
				throw std::runtime_error( "Failed to run auto_updater.exe" );
			}
		}
		catch (const std::runtime_error&) {
			MessageBox( nullptr, "Failed to get/run updater, try re-downloading the application if this persists.", "Update Failed", MB_ICONERROR | MB_OK );
		}
	}
}

void ItemManager::handle_callback( void ) {
	CallbackMsg_t callback;
	if ( steamItems_->get_callback( &callback ) ) {
		switch (callback.m_iCallback) {
		case GCMessageAvailable_t::k_iCallback:
			{
				GCMessageAvailable_t *message = (GCMessageAvailable_t *)callback.m_pubParam;
				
				uint32 size;
				if ( steamItems_->has_message( &size ) )
				{
					uint32 id, realSize = 0;

					// Retrieve the message.
					// WARNING: Do NOT use return before calling free on buffer.
					void* buffer = malloc( size );
					steamItems_->get_message( &id, buffer, size, &realSize );

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
							steamItems_->set_target_id( headerMsg.job_id_source() );
						}

						uint32 bodySize = size - sizeof( GCProtobufHeader_t ) - headerSize;
						handle_protobuf( realId, headerBuffer.here(), bodySize );
					}
					else {
						handle_message( id, buffer, size );
					}

					if (buffer != nullptr) {
						free( buffer );
						buffer = nullptr;
					}
				}
			}
		}

		steamItems_->release_callback();
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
			CMsgSOCacheSubscribed subscribedMsg;
			subscribedMsg.ParseFromArray( message, size );
			steamItems_->set_version( subscribedMsg.version() );

			// Check that this is our backpack.
			if (subscribedMsg.owner() != steamItems_->get_steam_id()) {
				break;
			}

			// Empty backpack.
			backpack_->empty_slots();
			backpack_->remove_items();
								
			// Add items.
			for (int i = 0; i < subscribedMsg.objects_size(); i++) {
				CMsgSOCacheSubscribed_SubscribedType subscribedType = subscribedMsg.objects( i );
				switch (subscribedType.type_id()) {
				case 1:
					{
						// Ensure we own this item.
						if (steamItems_->get_steam_id() == subscribedMsg.owner()) {
							for (int i = 0; i < subscribedType.object_data_size(); i++) {
								CSOEconItem econItem;
								econItem.ParseFromArray( subscribedType.object_data( i ).data(), subscribedType.object_data( i ).size() );
								Item *item = new Item(
									econItem.id(),
									econItem.def_index(),
									econItem.level(),
									(EItemQuality)econItem.quality(),
									econItem.quantity(),
									econItem.inventory(),
									econItem.origin() );

								// Add the item's attributes.
								for (int j = 0; j < econItem.attribute_size(); ++j) {
									const CSOEconItemAttribute& attribute = econItem.attribute( j );
									uint32 attrib_index = attribute.def_index();
									
									// Get attribute information.
									auto k = Item::attributes.find( attrib_index );
									if (k != Item::attributes.end()) {
										Attribute* new_attribute = new Attribute( k->second, attribute.value() );
										item->add_attribute( new_attribute );
									}
								}

								if (econItem.has_custom_name()) {
									item->set_custom_name( econItem.custom_name() );
								}

								backpack_->insert_item( item );
							}
						}

						break;
					}
				case 7:
					{
						if (steamItems_->get_steam_id() == subscribedMsg.owner()) {
							// TODO: Handle backpack expansions.
							for (int i = 0; i < subscribedType.object_data_size(); i++) {
								CSOEconGameAccountClient gameAccountClient;
								gameAccountClient.ParseFromArray( subscribedType.object_data( i ).data(), subscribedType.object_data( i ).size() );

								// Check how many slots this backpack is supposed to have.
								unsigned int slots = gameAccountClient.additional_backpack_slots();
								if (gameAccountClient.trial_account()) {
									slots += PAGE_WIDTH * PAGE_HEIGHT * TRIAL_PAGE_COUNT;
								}
								else {
									slots += PAGE_WIDTH * PAGE_HEIGHT * PAGE_COUNT;
								}

								if (slots < backpack_->GetInventorySize()) {
									throw std::runtime_error( "New backpack size smaller than old." );
								}

								// Add the newer slots.
								unsigned int added_slots = slots - backpack_->GetInventorySize();
								backpack_->add_slots( added_slots );
								inventoryView_->add_pages( backpack_->get_inventory_slots() );
								update_page_display();
							}
						}

						break;
					}
				}
			}

			// Attempt to place all excluded items.
			backpack_->resolve_excluded();
			backpack_->update_excluded();
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
			if (steamItems_->get_version() != version) {
				steamItems_->set_version( version );

				// Send refresh.
				CMsgSOCacheSubscriptionRefresh refresh;
				refresh.set_owner( steamItems_->get_steam_id() );
				string refreshString = refresh.SerializeAsString();
				steamItems_->send_message( 28 | 0x80000000, (void*)refreshString.c_str(), refreshString.size() );
			}
			break;
		}

	case k_ESOMsg_Update:
		{
			CMsgSOSingleObject updateMsg;
			updateMsg.ParseFromArray( message, size );
			steamItems_->set_version( updateMsg.version() );
			if (updateMsg.type_id() == 1) {
				CSOEconItem econItem;
				econItem.ParseFromArray( updateMsg.object_data().data(), updateMsg.object_data().size() );

				// Attempt to find the item.
				Item *item = backpack_->GetItemByUniqueId( econItem.id() );
				if (item == nullptr) {
					break;
				}

				// Place item into excluded, to be resolved later.
				backpack_->remove_item( item );
				backpack_->ToExcluded( item );
				item->set_flags( econItem.inventory() );
			}

			break;
		}

	case k_ESOMsg_UpdateMultiple:
		{
			CMsgSOMultipleObjects updateMsg;
			updateMsg.ParseFromArray( message, size );
			steamItems_->set_version( updateMsg.version() );

			for (int i = 0; i < updateMsg.objects_size(); i++) {
				CMsgSOMultipleObjects::SingleObject singleObject = updateMsg.objects( i );
				if (singleObject.type_id() == 1) {
					CSOEconItem econItem;
					econItem.ParseFromArray( singleObject.object_data().data(), singleObject.object_data().size() );

					// Attempt to find the item.
					Item *item = backpack_->GetItemByUniqueId( econItem.id() );
					if (item == nullptr) {
						break;
					}

					// Place item into excluded, to be resolved later.
					backpack_->remove_item( item );
					backpack_->ToExcluded( item );
					item->set_flags( econItem.inventory() );
				}
			}

			// Attempt to reposition excluded slots.
			backpack_->resolve_excluded();
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
			CMsgSOSingleObject deleteObj;
			deleteObj.ParseFromArray( message, size );
			steamItems_->set_version( deleteObj.version() );

			// Get item from object.
			CSOEconItem econItem;
			econItem.ParseFromArray( deleteObj.object_data().data(), deleteObj.object_data().size() );

			// Now add item.
			Item* newItem = new Item(
				econItem.id(),
				econItem.def_index(),
				econItem.level(),
				(EItemQuality)econItem.quality(),
				econItem.quantity(),
				econItem.inventory(),
				econItem.origin() );

			// Add this item to excluded.
			backpack_->insert_item( newItem );
			backpack_->update_excluded();
			
			// Get the source.
			string source;
			if (econItem.origin() == 4) {
				source = "crafted";
			}
			else {
				source = "found";
			}

			// Display message.
			notifications_->add_notification( "You have " + source + " a " + newItem->get_name() + ".", newItem->get_texture() );

			break;
		}

	case SOMsgDeleted_t::k_iMessage:
		{
			// Get deleted message.
			CMsgSOSingleObject deleteObj;
			deleteObj.ParseFromArray( message, size );
			steamItems_->set_version( deleteObj.version() );

			// Get ID of deleted item.
			CSOEconItem deletedItem;
			deletedItem.ParseFromArray( deleteObj.object_data().data(), deleteObj.object_data().size() );

			// Now remove from inventory.
			Item *targettedItem = backpack_->GetItemByUniqueId( deletedItem.id() );
			if (targettedItem != nullptr) {
				backpack_->remove_item( targettedItem );

				// Make sure the deleted item isn't being dragged.
				if (draggedView_ != nullptr) {
					Slot* draggedSlot = draggedView_->get_slot();
					if (draggedSlot->has_item()) {
						Item* item = draggedSlot->get_item();
						if (item == targettedItem) {
							remove( draggedView_ );
							delete draggedView_;
							draggedView_ = nullptr;
						}
					}
				}

				delete targettedItem;
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
			exit_application();
		}
		else if (popup == craft_check_) {
			if (craft_check_->get_response() == RESPONSE_YES) {
				steamItems_->craft_selected();
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
			exit_application();
		}
	}
}