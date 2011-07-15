#include "item_manager.h"

#include <algorithm>
#include <stdlib.h>
#include <crtdbg.h>
#include <math.h>
#include <json/json.h>

#include "serialized_buffer.h"
#include "slot_view.h"

#include "protobuf/base_gcmessages.pb.h"
#include "protobuf/steammessages.pb.h"
#include "protobuf/gcsdk_gcmessages.pb.h"

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#define D3D_DEBUG_INFO
#endif

// Temporarily disable equip and sort.
#define EQUIP_NOT_IMPLEMENTED
#define SORT_NOT_IMPLEMENTED

using namespace std;

// Application attributes.
const char*	APPLICATION_TITLE	= "Jengerer's Item Manager";
const int	APPLICATION_WIDTH	= 795;
const int	APPLICATION_HEIGHT	= 540;
const char*	APPLICATION_VERSION	= "1.0.0.0";

// UI attributes.
const unsigned int EXIT_BUTTON_PADDING	= 10;
const DWORD PAGE_DELAY_INTERVAL			= 500;

// Title display.
const char* TITLE_FONT_FACE				= "TF2 Build";
const unsigned int TITLE_FONT_SIZE		= 18;
const unsigned int TITLE_FONT_BOLDED	= false;
const D3DCOLOR TITLE_COLOUR				= D3DCOLOR_XRGB( 241, 239, 237);

// Inventory attributes.
const int PAGE_WIDTH		= 10;
const int PAGE_HEIGHT		= 5;
const int PAGE_COUNT		= 6;
const int EXCLUDED_SIZE		= 5;

const unsigned int PADDING	= 20;
const unsigned int SPACING	= 10;

LRESULT CALLBACK wndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message) {
	case WM_DESTROY:
		PostQuitMessage( 0 );
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
		if (itemManager != nullptr) {
			delete itemManager;
			itemManager = nullptr;
		}

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

			TranslateMessage( &msg );
			DispatchMessage( &msg );
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
	// Alerts and errors.
	alert_ = nullptr;
	error_ = nullptr;

	// Item stats.
	itemDisplay_ = nullptr;

	// Slot views.
	inventoryView_ = nullptr;
	excludedView_ = nullptr;

	// Dragged slot.
	dragTarget_ = nullptr;
	draggedView_ = nullptr;

	// Threaded loader.
	definitionLoader_ = nullptr;
	loadProgress_ = nullptr;

	// Fonts.
	titleFont_ = nullptr;

	// Create Steam interface.
	steamItems_ = new SteamItemHandler();

	// Create backpack.
	backpack_ = new Backpack( PAGE_WIDTH * PAGE_HEIGHT * PAGE_COUNT, EXCLUDED_SIZE );
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
	// Start up DirectX and window.
	Application::LoadInterfaces( APPLICATION_TITLE, instance );

	// Necessary to display progress/status.
	Notice::Precache( directX_ );
	Button::Precache( directX_ );

	try {
		// Start drawing to generate textures.
		directX_->BeginDraw();

		// Precache secondary resources.
		ItemDisplay::Precache( directX_ );
		Notification::Precache( directX_ );
		SlotView::Precache( directX_ );
		ToggleSet::Precache( directX_ );

		// End drawing.
		directX_->EndDraw();

		// Load title font.
		titleFont_ = directX_->CreateFont( TITLE_FONT_FACE, TITLE_FONT_SIZE, TITLE_FONT_BOLDED );

		// Create layout.
		VerticalLayout* layout = new VerticalLayout( SPACING, ALIGN_LEFT );

		// Create views.
		inventoryView_ = backpack_->CreateInventoryView( PAGE_WIDTH, PAGE_HEIGHT );
		excludedView_ = backpack_->CreateExcludedView();

		// Create button layout.
		Texture *craftTexture = directX_->GetTexture( "manager/gear" );
		Texture *equipTexture = directX_->GetTexture( "manager/equip" );
		Texture *sortTexture = directX_->GetTexture( "manager/sort" );

		// Create inventory buttons.
		craftButton_ = Button::CreateIconLabelButton( craftTexture, "craft" );
		equipButton_ = Button::CreateIconLabelButton( equipTexture, "equip" );
		sortButton_ = Button::CreateIconLabelButton( sortTexture, "sort" );
		craftButton_->SetEnabled( false );
		equipButton_->SetEnabled( false );
		sortButton_->SetEnabled( false );
	
		// Add to layout.
		HorizontalLayout* buttonLayout = new HorizontalLayout( BUTTON_SPACING );
		buttonLayout->Add( craftButton_ );
		buttonLayout->Add( equipButton_ );
		buttonLayout->Add( sortButton_ );
		buttonLayout->Pack();

		// Create title.
		stringstream titleStream;
		titleStream << APPLICATION_TITLE << " " << APPLICATION_VERSION;

		// Add version number.
		Text* titleText = new Text( titleFont_ );
		titleText->SetText( titleStream.str() );
		titleText->SetColour( TITLE_COLOUR );
		titleText->Pack();

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

		// Create notification queue.
		notifications_ = new NotificationQueue();
		notifications_->SetPosition( GetWidth() - PADDING, GetHeight() - PADDING );
		Add( notifications_ );

		// Create start up message.
		loadProgress_ = CreateNotice( "Initializing Steam interfaces..." );
		steamItems_->LoadInterfaces();

		// Attempt to load Steam and definitions.
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
		definitionLoader_->End();
		delete definitionLoader_;
		definitionLoader_ = nullptr;
	}

	// Erase item definitions.
	for (auto i = Item::definitions.begin(); i != Item::definitions.end(); i = Item::definitions.erase( i )) {
		delete i->second;
	}

	// Free cached resources.
	ItemDisplay::Release();
	Button::Release();
	Notice::Release();
	Notification::Release();
	SlotView::Release();
	ToggleSet::Release();

	// Free all protobuf library resources.
	google::protobuf::ShutdownProtobufLibrary();
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
		switch (definitionLoader_->GetState()){
		case LOADING_STATE_START:
			error_ = CreateAlert( "Definition loader was not started." );
			SetThink( &ItemManager::Exiting );
			break;

		case LOADING_STATE_RUNNING:
			{
				stringstream loadPercentage;
				loadPercentage << "Loading and downloading item definition resources... (" << floor(definitionLoader_->GetProgress() * 100.0f) << "%)";
				loadProgress_->SetMessage( loadPercentage.str() );
				loadProgress_->CenterTo( this );
			}
			break;

		case LOADING_STATE_ERROR:
			// Show error first.
			error_ = CreateAlert( definitionLoader_->GetErrorMsg() );

			// Remove threaded loader.
			definitionLoader_->End();
			delete definitionLoader_;
			definitionLoader_ = nullptr;
			SetThink( &ItemManager::Exiting );
			break;

		case LOADING_STATE_FINISHED:
			// Remove threaded loader.
			definitionLoader_->End();
			delete definitionLoader_;
			definitionLoader_ = nullptr;

			loadProgress_->SetMessage( "Waiting for Steam message..." );
			loadProgress_->CenterTo( this );
			break;
		}
	}
	else {
		HandleCallbacks();
		if (backpack_->IsLoaded()) {
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
	else {
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
			if (mouse->IsTouching( craftButton_ )) {
				return true;
			}
			else if (mouse->IsTouching( equipButton_ )) {
				return true;
			}
			else if (mouse->IsTouching( sortButton_ )) {
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
				ExitApplication();
			}

			HandlePopup( top );
			return true;
		}
	}
	else {
		if (draggedView_ != nullptr) {
			SlotView* slotView = inventoryView_->GetTouchingSlot( mouse );
			SlotReleased( slotView );
		}
		else {
			if (craftButton_->MouseReleased( mouse )) {
				steamItems_->CraftSelected();
				UpdateButtons();
			}
		}
	}

	return false;
}

bool ItemManager::MouseMoved( Mouse *mouse )
{
	// Reset item display.
	itemDisplay_->SetItem( nullptr );

	// Update buttons.
	// TODO: Have a button frame mouse hover state.
	craftButton_->MouseMoved( mouse );
	equipButton_->MouseMoved( mouse );
	sortButton_->MouseMoved( mouse );

	// Pass message to highest popup.
	if (!popups_.empty()) {
		Popup* top = popups_.back();
		top->MouseMoved( mouse );
	}
	else {
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
				}
				else if (draggedView_->GetX() + draggedView_->GetWidth() >= GetWidth()) {
					if (inventoryView_->NextPage()) {
						pageDelay_ = currentTick + PAGE_DELAY_INTERVAL;
					}
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
					itemDisplay_->SetPosition( 
						slotView->GetX() + (slotView->GetWidth() - itemDisplay_->GetWidth()) / 2.0f,
						slotView->GetY() + slotView->GetHeight() + PADDING );
					ClampChild( itemDisplay_, PADDING );
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

	// Check if item is excluded.
	bool isExcluded = !backpack_->CanInsert( draggedItem );

	// Return to target slot.
	Slot* targetSlot = dragTarget_->GetSlot();
	targetSlot->SetItem( draggedItem );

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
			if (!isExcluded) {
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
			}
			else if (IsKeyClicked( VK_LEFT )) {
				inventoryView_->PreviousPage();
			}
		}		
	}
}

void ItemManager::UpdateItemDisplay( void )
{
	itemDisplay_->UpdateAlpha();
}

void ItemManager::LoadDefinitions( void )
{
	// Set the message and redraw.
	loadProgress_->SetMessage("Loading item definitions...");

	// Set up loader.
	definitionLoader_ = new DefinitionLoader( directX_, "http://www.jengerer.com/itemManager/item_definitions.json" );
	definitionLoader_->Begin();
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
		jsonInventory = directX_->read(apiUrl);
	}
	catch (Exception curlException) {
		throw Exception("Failed to read inventory from profile.");
	}

	// Show success.
	loadProgress_->SetMessage("Items successfully loaded!");
	DrawFrame();

	backpack_->SetLoaded( true );
}

#include <fstream>
#include <iomanip>

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

void ItemManager::HandleProtobuf( uint32 id, void* message, size_t size )
{
	switch (id) {
	case k_ESOMsg_CacheSubscribed:
		{
			if (backpack_ != nullptr && backpack_->IsLoaded() ) {
				return;
			}

			CMsgSOCacheSubscribed subscribedMsg;
			subscribedMsg.ParseFromArray( message, size );
								
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
									econItem.inventory() );
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
							for (int i = 0; i < subscribedType.object_data_size(); i++) {
								CSOEconGameAccountClient gameAccountClient;
								gameAccountClient.ParseFromArray( subscribedType.object_data( i ).data(), subscribedType.object_data( i ).size() );
								backpack_->AddSlots( gameAccountClient.additional_backpack_slots() );
							}
						}

						break;
					}
				}
			}

			backpack_->SetLoaded( true );
			backpack_->UpdateExcluded();
			notifications_->AddNotification( "Backpack successfully loaded from Steam.", nullptr );
			break;
		}

	case k_EMsgGCStartupCheck:
		{
			// Receive the startup check.
			CMsgStartupCheck startupMsg;
			startupMsg.ParseFromArray( message, size );

			// Build a response.
			CMsgStartupCheckResponse responseMsg;
			responseMsg.set_item_schema_version( 0 );
			string responseString = responseMsg.SerializeAsString();

			// Send and free.
			steamItems_->SendMessage( k_EMsgGCStartupCheckResponse | 0x80000000, (void*)responseString.c_str(), responseString.size() );
		}
		break;

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
				econItem.inventory() );

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
		// steamItems_->HandleMessage( id & 0x0FFFFFFF, headerBuffer.here(), bodySize );
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