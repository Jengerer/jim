#include "item_manager.h"

#include <algorithm>
#include <stdlib.h>
#include <crtdbg.h>
#include <math.h>
#include <shellapi.h>
#include <tlhelp32.h>
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
	ItemManager* itemManager = new ItemManager();

	try {
		itemManager->LoadInterfaces( hInstance );
	}
	catch (Exception mainException) {
		MessageBox( NULL, mainException.getMessage()->c_str(), "Initialization failed!", MB_OK );
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

		itemManager->RunApplication();
	}

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
	layoutCreated_ = false;

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
	backpack_ = nullptr;

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
	SetThink( &ItemManager::Loading );

	// Listen for input keys.
	AddKey( VK_LEFT );
	AddKey( VK_RIGHT );
	AddKey( VK_ESCAPE );
	AddKey( VK_RETURN );
	AddKey( VK_CONTROL );
}

ItemManager::~ItemManager( void )
{
	CloseInterfaces();
}

void ItemManager::LoadInterfaces( HINSTANCE instance )
{
	// Start up Graphics2D and window.
	Application::LoadInterfaces( APPLICATION_TITLE, instance );

	// Necessary to display progress/status.
	FontFactory::initialize();
	Notice::Precache( graphics_ );
	Button::Precache( graphics_ );

	try {
		// Check for latest version.
		if (!IsLatestVersion()) {
			updateError_ = true;
			throw Exception( "A new version of the item manager is out and needs to be downloaded against your will. Press okay to continue." );
		}

		// Ensure TF2's not running.
		if (IsTF2Running()) {
			throw Exception( "Please close Team Fortress 2 before running the item manager." );
		}
		else if (IsManagerRunning()) {
			throw Exception( "Another instance of the item manager is already running!" );
		}

		// Precache secondary resources.
		ToggleSet::Precache();
		ItemDisplay::Precache();
		Notification::Precache();
		SlotView::Precache( graphics_ );

		// Load title font.
		titleFont_ = FontFactory::create_font( TITLE_FONT_FACE, TITLE_FONT_SIZE );
		pageFont_ = FontFactory::create_font( PAGE_FONT_FACE, PAGE_FONT_SIZE );

		// Create notification queue.
		notifications_ = new NotificationQueue();
		notifications_->SetPosition( 
			static_cast<float>(GetWidth() - PADDING), 
			static_cast<float>(GetHeight() - PADDING) );
		Add( notifications_ );

		// Start definition loader.
		loadProgress_ = CreateNotice( "Preparing to load item definitions..." );
		LoadDefinitions();
	}
	catch (Exception& loadException) {
		error_ = CreateAlert( *loadException.getMessage() );
		SetThink( &ItemManager::Exiting );
	}
}

void ItemManager::CloseInterfaces( void )
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

void ItemManager::CreateLayout( void )
{
	if (!layoutCreated_) {
		// Create layout.
		VerticalLayout* layout = new VerticalLayout( SPACING, ALIGN_LEFT );

		// Create views.
		inventoryView_ = backpack_->CreateInventoryView( PAGE_WIDTH, PAGE_HEIGHT );
		excludedView_ = backpack_->CreateExcludedView();

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
		inventoryButtons->Add( craftButton_ );
		inventoryButtons->Add( equipButton_ );
		inventoryButtons->Add( sortButton_ );
		inventoryButtons->Pack();

		// Create pages buttons/text.
		prevButton_ = Button::CreateLabelButton( "<" );
		nextButton_ = Button::CreateLabelButton( ">" );
		pageDisplay_ = new WrappedText( pageFont_, PAGE_LABEL_WIDTH );
		pageDisplay_->SetColour( PAGE_LABEL_COLOUR );
		pageDisplay_->SetTextFormatting( DT_CENTER );
		UpdatePageDisplay();

		// Create pages buttons layout.
		HorizontalLayout* pageButtons = new HorizontalLayout( BUTTON_SPACING );
		pageButtons->Add( prevButton_ );
		pageButtons->Add( pageDisplay_ ); // PAGE LABEL, RATHER
		pageButtons->Add( nextButton_ );
		pageButtons->Pack();

		HorizontalSplitLayout* buttonLayout = new HorizontalSplitLayout( inventoryView_->GetWidth() );
		buttonLayout->AddLeft( inventoryButtons);
		buttonLayout->AddRight( pageButtons );
		buttonLayout->Pack();
		
		// Create title.
		stringstream titleStream;
		titleStream << APPLICATION_TITLE << " " << APPLICATION_VERSION;

		// Add version number.
		Text* titleText = new Text( titleFont_ );
		titleText->SetText( titleStream.str() );
		titleText->SetColour( TITLE_COLOUR );

		// Organize layout.
		layout->Add( titleText );
		layout->Add( inventoryView_ );
		layout->Add( buttonLayout );
		layout->Add( excludedView_ );
		layout->Pack();
		layout->SetPosition( (GetWidth() - layout->GetWidth()) / 2.0f, 
			(GetHeight() - layout->GetHeight()) / 2.0f );
		Add( layout );

		// Create item display.
		itemDisplay_ = new ItemDisplay();
		Add( itemDisplay_ );

		layoutCreated_ = true;
	}
}

void ItemManager::RunApplication( void )
{
	Application::RunApplication();
	DoThink();
	DrawFrame();
}

void ItemManager::SetThink( void (ItemManager::*thinkFunction)( void ) )
{
	thinkFunction_ = thinkFunction;
}

void ItemManager::DoThink()
{
	(this->*thinkFunction_)();
}

void ItemManager::Loading()
{
	if (definitionLoader_ != nullptr) {
		switch (definitionLoader_->get_state()){
		case LOADING_STATE_ERROR:
			// Show error first.
			error_ = CreateAlert( definitionLoader_->get_progress_msg() );

			// Remove threaded loader.
			delete definitionLoader_;
			definitionLoader_ = nullptr;
			SetThink( &ItemManager::Exiting );
			break;

		case LOADING_STATE_FINISHED:
			// Remove threaded loader.
			delete definitionLoader_;
			definitionLoader_ = nullptr;

			// Initialize steam.
			try {
				steamItems_->LoadInterfaces();
			}
			catch (const Exception& ex) {
				error_ = CreateAlert( *ex.getMessage() );
				SetThink( &ItemManager::Exiting );
				break;
			}

			loadProgress_->SetMessage( "Waiting for Steam message..." );
			loadProgress_->CenterTo( this );
			break;

		default:
			definitionLoader_->update_progress_msg();
			loadProgress_->SetMessage( definitionLoader_->get_progress_msg() );
			loadProgress_->CenterTo( this );
			break;
		}
	}
	else {
		HandleCallbacks();
		if (backpack_ != nullptr && backpack_->IsLoaded()) {
			SetThink( &ItemManager::Running );
			RemovePopup( loadProgress_ );
#ifndef SORT_NOT_IMPLEMENTED
			sortButton_->SetEnabled( true );
#endif
		}
	}
}

void ItemManager::Running()
{
	HandleCallbacks();
	notifications_->UpdateNotifications();
	inventoryView_->UpdateView();
	UpdateItemDisplay();
}

void ItemManager::Exiting()
{
	// Just wait for exit.
}

bool ItemManager::MouseClicked( Mouse *mouse )
{
	// Mouse clicked.
	if (!popups_.empty()) {
		Popup* top = popups_.back();
		if (top->MouseClicked( mouse )) {
			return true;
		}
	}
	else if (layoutCreated_) {
		if (draggedView_ != nullptr) {
			return true;
		}

		SlotView* slotView = nullptr;
		bool touchedView = true;

		// Attempt to get a slot.
		if (mouse->IsTouching( inventoryView_ )) {
			slotView = inventoryView_->GetTouchingSlot( mouse );
		}
		else if (mouse->IsTouching( excludedView_ )) {
			slotView = excludedView_->GetTouchingSlot( mouse );
		}
		else {
			touchedView = false;
		}

		if (touchedView) {
			if (slotView != nullptr) {
				SlotClicked( slotView, mouse );
				return true;
			}
		}
		else if (!touchedView) {
			// TODO: Set a 'clicked target' pointer so we can't just release on button and trigger.
			if (mouse->IsTouching( craftButton_ ) ||
				mouse->IsTouching( equipButton_ ) ||
				mouse->IsTouching( sortButton_ ) ||
				mouse->IsTouching( nextButton_ ) ||
				mouse->IsTouching( prevButton_ )) {
					return true;
			}
			else if (notifications_->MouseClicked( mouse )) {
				return true;
			}
		}

		// Deselect all, nothing clicked.
		if (steamItems_->GetSelectMode() != SELECT_MODE_MULTIPLE) {
			steamItems_->DeselectAll();
			UpdateButtons();
		}
	}

	return false;
}

bool ItemManager::MouseReleased( Mouse *mouse )
{
	if (!popups_.empty()) {
		Popup *top = popups_.back();
		if (top->MouseReleased( mouse )) {
			// Check for error.
			if (top == error_ && top->GetState() == POPUP_STATE_KILLED) {
				if (updateError_) {
					LaunchUpdater();
				}

				ExitApplication();
			}

			HandlePopup( top );
			return true;
		}
	}
	else {
		if (layoutCreated_) {
			if (draggedView_ != nullptr) {
				SlotView* slotView = inventoryView_->GetTouchingSlot( mouse );
				SlotReleased( slotView );
			}
			else {
				if (craftButton_->MouseReleased( mouse )) {
					steamItems_->CraftSelected();
					UpdateButtons();
				}
				else if (nextButton_->MouseReleased( mouse )) {
					inventoryView_->NextPage();
					UpdatePageDisplay();
				}
				else if (prevButton_->MouseReleased( mouse )) {
					inventoryView_->PreviousPage();
					UpdatePageDisplay();
				}
			}
		}
	}

	return false;
}

bool ItemManager::MouseMoved( Mouse *mouse )
{
	// Update buttons.
	// TODO: Have a button frame mouse hover state.
	if (layoutCreated_) {
		itemDisplay_->SetItem( nullptr );

		craftButton_->MouseMoved( mouse );
		equipButton_->MouseMoved( mouse );
		sortButton_->MouseMoved( mouse );
		nextButton_->MouseMoved( mouse );
		prevButton_->MouseMoved( mouse );
	}

	// Pass message to highest popup.
	if (!popups_.empty()) {
		Popup* top = popups_.back();
		top->MouseMoved( mouse );
	}
	else if (layoutCreated_) {
		// Check if dragging.
		if (draggedView_ != nullptr) {
			draggedView_->MouseMoved( mouse );
			ClampChild( draggedView_ );

			// Check if we're switching page.
			DWORD currentTick = GetTickCount();
			if (currentTick > pageDelay_) {
				if (draggedView_->GetX() <= 0.0f) {
					if (inventoryView_->PreviousPage()) {
						pageDelay_ = currentTick + PAGE_DELAY_INTERVAL;
					}

					UpdatePageDisplay();
				}
				else if (draggedView_->GetX() + draggedView_->GetWidth() >= GetWidth()) {
					if (inventoryView_->NextPage()) {
						pageDelay_ = currentTick + PAGE_DELAY_INTERVAL;
					}

					UpdatePageDisplay();
				}
			}	
		}
		else {
			SlotView* slotView = nullptr;
			if (mouse->IsTouching( inventoryView_ )) {
				slotView = inventoryView_->GetTouchingSlot( mouse );
			}
			else if (mouse->IsTouching( excludedView_ )) {
				slotView = excludedView_->GetTouchingSlot( mouse );
			}
			else {
				return false;
			}
			
			if (slotView != nullptr) {
				Slot* slot = slotView->GetSlot();
				if (slot->HasItem()) {
					itemDisplay_->SetItem( slot->GetItem() );
					float displayX = slotView->GetX() + (slotView->GetWidth() - itemDisplay_->GetWidth()) / 2.0f;
					float displayY = slotView->GetY() + slotView->GetHeight() + ITEM_DISPLAY_SPACING;
					if (displayY + itemDisplay_->GetHeight() > GetHeight()) {
						displayY = slotView->GetY() - itemDisplay_->GetHeight() - ITEM_DISPLAY_SPACING;
					}

					itemDisplay_->SetPosition(
						static_cast<float>(displayX),
						static_cast<float>(displayY) );
					ClampChild( itemDisplay_, static_cast<float>(PADDING) );
				}
			}
		}

		return true;
	}

	return false;
}

void ItemManager::SlotClicked( SlotView* slotView, Mouse* mouse )
{
	Slot* slot = slotView->GetSlot();
	if (slot->HasItem()) {
		switch (steamItems_->GetSelectMode()) {
		case SELECT_MODE_SINGLE:
			{
				assert( draggedView_ == nullptr );
				Item* item = slot->GetItem();
				steamItems_->DeselectAll();

				// Start dragging.
				dragTarget_ = slotView;
				float viewX = slotView->GetX();
				float viewY = slotView->GetY();
				draggedView_ = new DraggedSlotView( new Slot );
				draggedView_->SetPosition( viewX, viewY );
				draggedView_->SetOffset( viewX - mouse->GetX(), viewY - mouse->GetY() );
				draggedView_->SetAlpha( 200 );
				steamItems_->Select( draggedView_ );

				// Swap items.
				draggedView_->GetSlot()->SetItem( item );
				slot->SetItem( nullptr );

				Add( draggedView_ );
				break;
			}

		case SELECT_MODE_MULTIPLE:
			steamItems_->ToggleSelect( slotView );
			break;
		}
	}
	else if (steamItems_->GetSelectMode() != SELECT_MODE_MULTIPLE) {
		steamItems_->DeselectAll();
	}

	UpdateButtons();
}

void ItemManager::SlotReleased( SlotView* slotView )
{
	Slot* draggedSlot = draggedView_->GetSlot();
	Item* draggedItem = draggedSlot->GetItem();
	backpack_->RemoveItem( draggedItem );
	steamItems_->DeselectAll();

	// Move back if not excluded.
	bool isExcluded = !backpack_->CanInsert( draggedItem );
	Slot* targetSlot = dragTarget_->GetSlot();

	// Delete temporary dragged.
	SlotView* selectTarget = dragTarget_;
	Remove( draggedView_ );
	delete draggedView_;
	draggedView_ = nullptr;
	dragTarget_ = nullptr;

	bool toInventory = false;
	if (slotView != nullptr) {
		// Now try move slot.
		Slot* touchedSlot = slotView->GetSlot();
		if (touchedSlot->HasItem()) {
			Item* touchedItem = touchedSlot->GetItem();
			if (!isExcluded && targetSlot != touchedSlot) {
				// Swap slots.
				targetSlot->SetItem( touchedItem );
				touchedSlot->SetItem( draggedItem );
				draggedItem->SetPosition( draggedItem->GetIndex() );
				touchedItem->SetPosition( touchedItem->GetIndex() );
				steamItems_->UpdateItem( touchedItem );
				steamItems_->UpdateItem( draggedItem );
				selectTarget = slotView;
				toInventory = true;
			}
		}
		else {
			targetSlot->SetItem( nullptr );
			touchedSlot->SetItem( draggedItem );
			draggedItem->SetPosition( draggedItem->GetIndex() );
			steamItems_->UpdateItem( draggedItem );
			selectTarget = slotView;
			toInventory = true;
		}
	}
	else if (!isExcluded) {
		targetSlot->SetItem( draggedItem );
		toInventory = true;
	}

	// Move item to appropriate group.
	if (toInventory) {
		backpack_->ToInventory( draggedItem );
	}
	else {
		backpack_->ToExcluded( draggedItem );
	}

	steamItems_->Select( selectTarget );
	backpack_->UpdateExcluded();
	UpdateButtons();
}

void ItemManager::UpdateButtons()
{
	unsigned int selectedCount = steamItems_->GetSelectedCount();
	craftButton_->SetEnabled( selectedCount != 0 );

#ifndef EQUIP_NOT_IMPLEMENTED
	equipButton_->SetEnabled( steamItems_->CanEquipSelected() );
#endif
}

void ItemManager::HandleKeyboard( void )
{
	if (IsKeyPressed( VK_ESCAPE )) {
		ExitApplication();
	}
	else {
		if (!popups_.empty()) {
			Popup *top = popups_.back();
			if (top == error_) {
				if (IsKeyPressed( VK_RETURN )) {
					if (updateError_) {
						LaunchUpdater();
					}

					ExitApplication();
					RemovePopup( top );
				}
			}
		}
		else {
			if (IsKeyClicked( VK_CONTROL )) {
				steamItems_->SetSelectMode( SELECT_MODE_MULTIPLE );
			}
			else if (this->IsKeyReleased( VK_CONTROL )) {
				steamItems_->SetSelectMode( SELECT_MODE_SINGLE );
			}
	
			if (IsKeyClicked( VK_RIGHT )) {
				inventoryView_->NextPage();
				UpdatePageDisplay();
			}
			else if (IsKeyClicked( VK_LEFT )) {
				inventoryView_->PreviousPage();
				UpdatePageDisplay();
			}
		}		
	}
}

void ItemManager::UpdateItemDisplay( void )
{
	itemDisplay_->UpdateAlpha();
}

void ItemManager::UpdatePageDisplay( void )
{
	// Update text.
	stringstream pageText;
	pageText << (inventoryView_->GetPageIndex() + 1) << '/' << inventoryView_->GetPageCount();
	pageDisplay_->SetText( pageText.str() );

	// Update buttons.
	unsigned int pageIndex = inventoryView_->GetPageIndex();
	unsigned int lastIndex = inventoryView_->GetPageCount() - 1;
	prevButton_->SetEnabled( pageIndex != 0 );
	nextButton_->SetEnabled( pageIndex != lastIndex );
}

void ItemManager::LoadDefinitions( void )
{
	// Set the message and redraw.
	loadProgress_->SetMessage("Loading item definitions...");

	// Set up loader.
	definitionLoader_ = new DefinitionLoader( graphics_ );
	definitionLoader_->begin();
}

void ItemManager::LoadItemsFromWeb( void )
{
	loadProgress_->AppendMessage("\n\nLoading items...");
	DrawFrame();

	uint64 userId = steamItems_->GetSteamId();
	stringstream urlStream;
	urlStream << "http://api.steampowered.com/ITFItems_440/GetPlayerItems/v0001/?key=0270F315C25E569307FEBDB67A497A2E&SteamID=" << userId << "&format=json";
	string apiUrl = urlStream.str();

	// Attempt to read the file.
	string jsonInventory;
	try {
		Curl* curl = Curl::get_instance();
		jsonInventory = curl->read( apiUrl );
	}
	catch (Exception curlException) {
		throw Exception("Failed to read inventory from profile.");
	}

	// Show success.
	loadProgress_->SetMessage("Items successfully loaded!");
	DrawFrame();

	backpack_->SetLoaded( true );
}

bool ItemManager::IsLatestVersion() const
{
	// Check for program updates.
	try {
		Curl* curl = Curl::get_instance();
		string versionInfo = curl->read( "http://www.jengerer.com/item_manager/item_manager.txt" );
		return versionInfo == APPLICATION_VERSION;
	}
	catch (const Exception&) {
		// Failed to get version, allow it.
	}

	return true;
}

void ItemManager::LaunchUpdater() const
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
				throw Exception( "Failed to run auto_updater.exe" );
			}
		}
		catch (const Exception&) {
			MessageBox( nullptr, "Failed to get/run updater, try re-downloading the application if this persists.", "Update Failed", MB_ICONERROR | MB_OK );
		}
	}
}

bool ItemManager::IsTF2Running() const
{
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof( PROCESSENTRY32 );
	HANDLE snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if (Process32First( snapshot, &entry )) {
		while (Process32Next( snapshot, &entry )) {
			if (strcmp( entry.szExeFile, "hl2.exe" ) == 0) {
				return true;
			}
		}
	}

	return false;
}

bool ItemManager::IsManagerRunning() const
{
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof( PROCESSENTRY32 );
	HANDLE snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	bool foundFirst = false;
	if (Process32First( snapshot, &entry )) {
		while (Process32Next( snapshot, &entry )) {
			if (strcmp( entry.szExeFile, "item_manager.exe" ) == 0) {
				if (foundFirst) {
					return true;
				}
				else {
					foundFirst = true;
				}
			}
		}
	}

	return false;
}

void ItemManager::HandleCallbacks( void ) {
	CallbackMsg_t callback;
	if ( steamItems_->GetCallback( &callback ) ) {
		switch (callback.m_iCallback) {
		case GCMessageAvailable_t::k_iCallback:
			{
				GCMessageAvailable_t *message = (GCMessageAvailable_t *)callback.m_pubParam;
				
				uint32 size;
				if ( steamItems_->HasMessage( &size ) )
				{
					uint32 id, realSize = 0;

					// Retrieve the message.
					// WARNING: Do NOT use return before calling free on buffer.
					void* buffer = malloc( size );
					steamItems_->GetMessage( &id, buffer, size, &realSize );

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
						notifications_->AddNotification( protoMsg.str(), nullptr );
#endif

						// Now get the real protobuf header.
						CMsgProtoBufHeader headerMsg;
						void *headerBytes = headerBuffer.here();
						headerMsg.ParseFromArray( headerBytes, headerSize );
						headerBuffer.push( headerSize );

						// Check if we can set target ID.
						// TODO: Maybe move all this horseshit into Steam.
						if ( headerMsg.has_job_id_source() ) {
							steamItems_->SetTargetId( headerMsg.job_id_source() );
						}

						uint32 bodySize = size - sizeof( GCProtobufHeader_t ) - headerSize;
						HandleProtobuf( realId, headerBuffer.here(), bodySize );
					}
					else {
						HandleMessage( id, buffer, size );
					}

					if (buffer != nullptr) {
						free( buffer );
						buffer = nullptr;
					}
				}
			}
		}

		steamItems_->ReleaseCallback();
	} 
}

void ItemManager::HandleMessage( uint32 id, void* message, size_t size )
{
	switch (id) {
		case GCCraftResponse_t::k_iMessage:
		{
			GCCraftResponse_t *craftMsg = static_cast<GCCraftResponse_t*>(message);
			if (craftMsg->blueprint == 0xffff) {
				notifications_->AddNotification( "Crafting failed. No such blueprint!", nullptr );
			}

			break;
		}
	}
}

#include <fstream>

void ItemManager::HandleProtobuf( uint32 id, void* message, size_t size )
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
			bool first_cache = (backpack_ == nullptr);
			if (first_cache) {
				backpack_ = new Backpack( EXCLUDED_SIZE );
			}
			else {
				backpack_->RemoveSlots();
				backpack_->EmptySlots();
			}

			// Create empty backpack.
			CMsgSOCacheSubscribed subscribedMsg;
			subscribedMsg.ParseFromArray( message, size );
			steamItems_->SetVersion( subscribedMsg.version() );
			unsigned int slotCount = PAGE_WIDTH * PAGE_HEIGHT * PAGE_COUNT;
								
			// TODO: Check for other users' backpack.
			for (int i = 0; i < subscribedMsg.objects_size(); i++) {
				CMsgSOCacheSubscribed_SubscribedType subscribedType = subscribedMsg.objects( i );
				switch (subscribedType.type_id()) {
				case 1:
					{
						if (steamItems_->GetSteamId() == subscribedMsg.owner()) {
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
									item->SetCustomName( econItem.custom_name() );
								}

								backpack_->InsertItem( item );
							}
						}

						break;
					}
				case 7:
					{
						if (steamItems_->GetSteamId() == subscribedMsg.owner()) {
							// TODO: Handle backpack expansions.
							if (first_cache) {
								for (int i = 0; i < subscribedType.object_data_size(); i++) {
									CSOEconGameAccountClient gameAccountClient;
									gameAccountClient.ParseFromArray( subscribedType.object_data( i ).data(), subscribedType.object_data( i ).size() );

									// Adjust slot count based on account type and slot additions.
									if (gameAccountClient.trial_account()) {
										backpack_->AddSlots( PAGE_WIDTH * PAGE_HEIGHT * TRIAL_PAGE_COUNT );
									}
									else {
										backpack_->AddSlots( PAGE_WIDTH * PAGE_HEIGHT * PAGE_COUNT );
									}

									backpack_->AddSlots( gameAccountClient.additional_backpack_slots() );
								}
							}
						}

						break;
					}
				}
			}

			// Attempt to place all excluded items.
			backpack_->ResolveExcluded();
			backpack_->UpdateExcluded();
			backpack_->SetLoaded( true );

			// Create layout if this is the first cache.
			if (first_cache) {
				CreateLayout();
				notifications_->AddNotification( "Backpack successfully loaded from Steam.", nullptr );
			}

			break;
		}

	case 27:
		{
			CMsgSOCacheSubscriptionCheck check;
			check.ParseFromArray( message, size );

			// Compare version.
			uint64 version = check.version();
			if (steamItems_->GetVersion() != version) {
				steamItems_->SetVersion( version );

				// Send refresh.
				CMsgSOCacheSubscriptionRefresh refresh;
				refresh.set_owner( steamItems_->GetSteamId() );
				string refreshString = refresh.SerializeAsString();
				steamItems_->SendMessage( 28 | 0x80000000, (void*)refreshString.c_str(), refreshString.size() );
			}
			break;
		}

	case k_ESOMsg_Update:
		{
			CMsgSOSingleObject updateMsg;
			updateMsg.ParseFromArray( message, size );
			steamItems_->SetVersion( updateMsg.version() );
			if (updateMsg.type_id() == 1) {
				CSOEconItem econItem;
				econItem.ParseFromArray( updateMsg.object_data().data(), updateMsg.object_data().size() );

				// Attempt to find the item.
				Item *item = backpack_->GetItemByUniqueId( econItem.id() );
				if (item == nullptr) {
					break;
				}

				// Place item into excluded, to be resolved later.
				backpack_->RemoveItem( item );
				backpack_->ToExcluded( item );
				item->SetFlags( econItem.inventory() );
			}

			break;
		}

	case k_ESOMsg_UpdateMultiple:
		{
			CMsgSOMultipleObjects updateMsg;
			updateMsg.ParseFromArray( message, size );
			steamItems_->SetVersion( updateMsg.version() );

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
					backpack_->RemoveItem( item );
					backpack_->ToExcluded( item );
					item->SetFlags( econItem.inventory() );
				}
			}

			// Attempt to reposition excluded slots.
			backpack_->ResolveExcluded();
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
			steamItems_->SetVersion( deleteObj.version() );

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
			backpack_->InsertItem( newItem );
			backpack_->UpdateExcluded();
			
			// Get the source.
			string source;
			if (econItem.origin() == 4) {
				source = "crafted";
			}
			else {
				source = "found";
			}

			// Display message.
			notifications_->AddNotification( "You have " + source + " a " + newItem->GetName() + ".", newItem->GetTexture() );

			break;
		}

	case SOMsgDeleted_t::k_iMessage:
		{
			// Get deleted message.
			CMsgSOSingleObject deleteObj;
			deleteObj.ParseFromArray( message, size );
			steamItems_->SetVersion( deleteObj.version() );

			// Get ID of deleted item.
			CSOEconItem deletedItem;
			deletedItem.ParseFromArray( deleteObj.object_data().data(), deleteObj.object_data().size() );

			// Now remove from inventory.
			Item *targettedItem = backpack_->GetItemByUniqueId( deletedItem.id() );
			if (targettedItem != nullptr) {
				backpack_->RemoveItem( targettedItem );

				// Make sure the deleted item isn't being dragged.
				if (draggedView_ != nullptr) {
					Slot* draggedSlot = draggedView_->GetSlot();
					if (draggedSlot->HasItem()) {
						Item* item = draggedSlot->GetItem();
						if (item == targettedItem) {
							Remove( draggedView_ );
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

Notice* ItemManager::CreateNotice( const string& message )
{
	Notice* notice = new Notice( message );
	notice->CenterTo( this );
	ShowPopup( notice );
	return notice;
}

Alert* ItemManager::CreateAlert( const string& message )
{
	Alert* alert = new Alert( message );
	alert->CenterTo( this );
	ShowPopup( alert );
	return alert;
}

void ItemManager::ShowPopup( Popup* popup )
{
	popup->SetState( POPUP_STATE_ACTIVE );
	popups_.push_back( popup );
	Add( popup );
}

void ItemManager::HidePopup( Popup* popup )
{
	Remove( popup );
	vector<Popup*>::iterator i = find( popups_.begin(), popups_.end(), popup );
	if (i != popups_.end()) {
		popups_.erase( i );
	}
}

void ItemManager::RemovePopup( Popup* popup )
{
	HidePopup( popup );
	delete popup;
}

void ItemManager::HandlePopup( Popup* popup )
{
	switch (popup->GetState()) {
	case POPUP_STATE_KILLED:
		RemovePopup( popup );
		break;
	case POPUP_STATE_HIDDEN:
		HidePopup( popup );
		break;
	}
}