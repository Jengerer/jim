#include "ItemManager.h"

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#endif

#include <stdlib.h>
#include <crtdbg.h>

#include "protobuf/base_gcmessages.pb.h"
#include "protobuf/steammessages.pb.h"
#include "protobuf/gcsdk_gcmessages.pb.h"

using namespace std;

// Window properties.
const char*	APPLICATION_TITLE	= "Jengerer's Item Manager";
const int	APPLICATION_WIDTH	= 795;
const int	APPLICATION_HEIGHT	= 540;

// Application attributes.
const float	APPLICATION_FRAMERATE	= 30.0f;
const float APPLICATION_FRAMESPEED	= (1000.0f / APPLICATION_FRAMERATE);
const int	APPLICATION_VERSION		= 1000;

// Inventory attributes.
#define PAGE_WIDTH				10
#define PAGE_HEIGHT				5
#define PAGE_COUNT				6
#define EXCLUDED_SIZE			5

LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	_crtBreakAlloc = 1421;

	switch (message) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	ItemManager* itemManager = new ItemManager();

	try {
		itemManager->LoadInterfaces( hInstance );
	}
	catch (Exception mainException) {
		MessageBox(NULL, mainException.getMessage()->c_str(), "Initialization failed!", MB_OK);
		return EXIT_FAILURE;
	}

	bool isDone = false;

	MSG msg;
	while (!isDone) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				isDone = true;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		itemManager->RunApplication();
	}

	if (itemManager) {
		delete itemManager;
		itemManager = 0;
	}

#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
#endif

	return EXIT_SUCCESS;
}

ItemManager::ItemManager( void ) : Application( APPLICATION_WIDTH, APPLICATION_HEIGHT )
{
	alert_ = nullptr;
	error_ = nullptr;
	backpack_ = nullptr;
	itemDisplay_ = nullptr;

	// Listen for input keys.
	AddKey( VK_LEFT );
	AddKey( VK_RIGHT );
	AddKey( VK_ESCAPE );
	AddKey( VK_RETURN );
	AddKey( VK_LCONTROL );
}

ItemManager::~ItemManager( void )
{
	CloseInterfaces();
}

void ItemManager::LoadInterfaces( HINSTANCE instance )
{
	// Start up DirectX and window.
	Application::LoadInterfaces( APPLICATION_TITLE, instance );

	// Start drawing to generate textures.
	directX_->BeginDraw();

	// Necessary to display progress/status.
	Notice::Precache( directX_ );
	Button::Precache( directX_ );

	try {
		// Precache secondary resources.
		ItemDisplay::Precache( directX_ );
		Notification::Precache( directX_ );
		Slot::Precache( directX_ );

		// End drawing.
		directX_->EndDraw();

		// Create start up message.
		loadProgress_ = CreateNotice( "Initializing item manager..." );

		// Create buttons.
		Texture *craftTexture = directX_->GetTexture( "manager/gear" );
		Texture *equipTexture = directX_->GetTexture( "manager/equip" );
		Texture *sortTexture = directX_->GetTexture( "manager/sort" );

		// Add all buttons to a layout.
		HorizontalLayout *buttonLayout = new HorizontalLayout();
		buttonLayout->SetSpacing( BUTTON_SPACING );
		buttonLayout->SetPosition( BACKPACK_PADDING, BUTTON_Y );
		craftButton_ = CreateButton( "craft", craftTexture, BACKPACK_PADDING, BUTTON_Y );
		equipButton_ = CreateButton( "equip", equipTexture, craftButton_->GetX() + craftButton_->GetWidth() + BUTTON_SPACING, BUTTON_Y );
		sortButton_	 = CreateButton( "sort", sortTexture, equipButton_->GetX() + equipButton_->GetWidth() + BUTTON_SPACING, BUTTON_Y );
		buttonLayout->Add( craftButton_ );
		buttonLayout->Add( equipButton_ );
		buttonLayout->Add( sortButton_ );
		buttonLayout->Pack();

		// Create backpack.
		backpack_ = new Backpack( 0.0f, 0.0f, this );
		backpack_->CreateInventory( PAGE_WIDTH, PAGE_HEIGHT, PAGE_COUNT, EXCLUDED_SIZE );
		backpack_->LoadInterfaces();
		AddBottom( backpack_ );
		AddBottom( buttonLayout );

		// Create item display.
		itemDisplay_ = new ItemDisplay();
		Add( itemDisplay_ );

		// Create notification queue.
		notifications_ = new NotificationQueue();
		notifications_->SetPosition( GetWidth() - BACKPACK_PADDING, GetHeight() - BACKPACK_PADDING );
		backpack_->SetNotificationQueue( notifications_ );
		Add( notifications_ );

		// Load definitions from translated APIs.
		LoadDefinitions();

		// Set default button state.
		sortButton_->SetEnabled( false );
		equipButton_->SetEnabled( false );
		craftButton_->SetEnabled( false );

		// We should be good to go!
		SetState( APPLICATION_STATE_RUN );
	}
	catch (Exception& loadException) {
		error_ = CreateAlert( *loadException.getMessage() );
		SetState( APPLICATION_STATE_EXIT );
	}

	// Safely finished, remove status.
	RemovePopup( loadProgress_ );
}

void ItemManager::CloseInterfaces( void )
{
	// Delete the mouse.
	if (mouse_ != nullptr) {
		delete mouse_;
		mouse_ = nullptr;
	}

	// Delete item information.
	if (Item::informationTable != nullptr) {
		delete Item::informationTable;
		Item::informationTable = nullptr;
	}

	// Free cached resources.
	// TODO: Make an IPrecachable interface and keep a list.
	Button::Release();
	ItemDisplay::Release();
	Notice::Release();
	Notification::Release();
	Slot::Release();

	// Free all protobuf library resources.
	google::protobuf::ShutdownProtobufLibrary();
}

void ItemManager::RunApplication( void )
{
	Application::RunApplication();
	if (GetState() == APPLICATION_STATE_RUN) {
		HandleCallbacks();
		backpack_->HandleCamera();

		notifications_->UpdateNotifications();
		UpdateItemDisplay();
	}

	// Redraw screen.
	DrawFrame();
}

bool ItemManager::OnLeftClicked( Mouse *mouse )
{
	// Mouse clicked.
	if (!popupStack_.empty()) {
		Popup* top = popupStack_.back();
		top->OnLeftClicked(mouse);
		HandlePopup( top );
		return true;
	}
	else {
		// Check, but don't register buttons.
		vector<Button*>::iterator i;
		for (i = buttonList_.begin(); i != buttonList_.end(); i++) {
			if (mouse->isTouching( *i )) {
				return true;
			}
		}

		// Check backpack.
		if (backpack_->OnLeftClicked(mouse)) {
			slotVector* selected = backpack_->GetSelected();

			// Set equip button state.
			if (selected->size() == 1) {
				Slot *slot = selected->at( 0 );
				Item *item = slot->GetItem();
				Hashtable *classes = item->GetEquipClasses();
				equipButton_->SetEnabled( classes != nullptr );
			}
			else {
				equipButton_->SetEnabled( false );
			}

			// Set craft button state.
			craftButton_->SetEnabled( selected->size() != 0 );
			return true;
		}

		craftButton_->SetEnabled( false );
		equipButton_->SetEnabled( false );
	}
	return false;
}

bool ItemManager::OnLeftReleased( Mouse *mouse )
{
	// TODO: Make popup handler with return codes like exit, close popup, etc.
	// Check top popup.
	if (!popupStack_.empty()) {
		Popup *top = popupStack_.back();
		if (mouse->isTouching( top )) {
			top->OnLeftReleased( mouse );
		}

		// Check if the popup has been closed.
		if (top == error_ && top->getState() == POPUP_STATE_KILLED) {
			ExitApplication();
		}

		// Handle removing and hiding.
		HandlePopup( top );
	}
	else {
		// Check backpack.
		if (backpack_->OnLeftReleased(mouse)) {
			return true;
		}

		// Now run buttons.
		if (craftButton_->IsEnabled() && mouse->isTouching(craftButton_)) {
			backpack_->CraftSelected();
			return true;
		}
		else if (equipButton_->IsEnabled() && mouse->isTouching(equipButton_)) {
			slotVector* selected = backpack_->GetSelected();
			Slot* slot = selected->at(0);
			Item* item = slot->GetItem();
			Hashtable* classes = item->GetEquipClasses();
			if (classes->size() > 1) {
				// Show equip menu.
				
			}
			else if (classes->size() == 1) {
				// Get the class.
				string className = classes->begin()->first;
				// TODO: Use some integer or enum to handle classes, not strings.
				backpack_->EquipItem(item, className);
			}
		}
	}

	return false;
}

bool ItemManager::OnRightClicked( Mouse *mouse )
{
	return true;
}

bool ItemManager::OnRightReleased( Mouse *mouse )
{
	return true;
}

bool ItemManager::OnMouseMoved( Mouse *mouse )
{
	// Get new position.
	mouse_->pollPosition();
	SetCursor( arrow_ );

	// Pass message to highest popup.
	if (!popupStack_.empty()) {
		Popup* top = popupStack_.back();
		top->OnMouseMoved( mouse );
	}
	else {
		// Check backpack.
		if (backpack_ && backpack_->OnMouseMoved(mouse)) {
			if ( backpack_->IsHovering() ) {
				SetCursor( hand_ );
			}

			return true;
		}

		// Check all buttons.
		vector<Button*>::iterator i;
		bool hitButton = false;
		for (i = buttonList_.begin(); i != buttonList_.end(); i++) {
			Button *button = *i;
			if (button->IsEnabled() && button->OnMouseMoved( mouse )) {
				SetCursor( hand_ );
				return true;
			}
		}
	}

	return false;
}

void ItemManager::HandleKeyboard( void )
{
	if (!popupStack_.empty()) {
		Popup *top = popupStack_.back();
		if (IsKeyPressed( VK_RETURN )) {
			if (top == error_) {
				ExitApplication();
			}
			RemovePopup( top );
		}
	}
	else {
		if (IsKeyPressed( VK_ESCAPE )) {
			ExitApplication();
		}
		else {
			if (backpack_ && backpack_->IsLoaded()) {
				// Toggle between single and multiple selection.
				backpack_->SetSelectMode( IsKeyPressed( VK_LCONTROL ) ? SELECT_MODE_MULTIPLE : SELECT_MODE_SINGLE );

				if (IsKeyClicked( VK_LEFT )) {
					backpack_->PrevPage();
				}
				else if (IsKeyClicked( VK_RIGHT )) {
					backpack_->NextPage();
				}
			}
		}
	}
}

void ItemManager::UpdateItemDisplay( void )
{
	if ( backpack_->IsHovering() ) {
		const Slot *hovering = backpack_->GetHovering();
		itemDisplay_->SetActive( true );
		itemDisplay_->SetItem( hovering->GetItem() );

		// Display position: horizontally centered and below the slot.
		float displayX = hovering->GetX() + hovering->GetWidth() / 2 - itemDisplay_->GetWidth() / 2;
		float displayY = hovering->GetY() + hovering->GetHeight() + ITEM_DISPLAY_SPACING;
		itemDisplay_->SetPosition( displayX, displayY );
		ClampChild( itemDisplay_, ITEM_DISPLAY_SPACING );
	}
	else {
		itemDisplay_->SetActive( false );
	}

	itemDisplay_->UpdateAlpha();
}

void ItemManager::LoadDefinitions( void )
{
	// Set the message and redraw.
	loadProgress_->SetMessage("Loading item definitions...");
	DrawFrame();

	// Load the item definitions.
	string itemDefinitions = directX_->read( "http://www.jengerer.com/itemManager/itemDefinitions.json" );

	// Begin parsing.
	Json::Reader	reader;
	Json::Value		root;
	if (!reader.parse(itemDefinitions, root, false))
		throw Exception("Failed to parse item definitions.");

	unsigned int texturesLoaded = 0;
	unsigned int textureCount = root.size();
	Item::informationTable = new Hashtable();
	for (Json::ValueIterator i = root.begin(); i != root.end(); i++) {
		Json::Value thisItem = *i;
		
		bool hasKeys = thisItem.isMember("index") &&
			thisItem.isMember("name") &&
			thisItem.isMember("slot") &&
			thisItem.isMember("image");

		if (!hasKeys) {
			throw Exception("Failed to parse item definitions. One or more missing members from item entry.");
		}

		// Get strings.
		string index	= thisItem.get( "index", root ).asString();
		string name		= thisItem.get( "name", root ).asString();
		string image	= thisItem.get( "image", root ).asString();
		string slot		= thisItem.get( "slot", root ).asString();

		// Make sure there's a file.
		if (image.length() == 0) {
			image = "backpack/unknown_item";
		}

		// Add strings to new table.
		Hashtable *itemTable = new Hashtable();
		itemTable->put("name", new string(name));
		itemTable->put("image", new string(image));
		itemTable->put("slot", new string(slot));

		if (thisItem.isMember("classes")) {
			Hashtable *classTable = new Hashtable();
			itemTable->put("classes", classTable);

			// Add all classes.
			Json::Value classes = thisItem.get("classes", root);
			for (Json::ValueIterator j = classes.begin(); j != classes.end(); j++) {
				string className = (*j).asString();
				classTable->put(className, new string("yes"));
			}
		}

		try {
			// Get the texture, add to table.
			Texture* texture = directX_->GetTexture( image );
			itemTable->put( "texture", texture );
		}
		catch (Exception &textureException) {
			if (itemTable != nullptr) {
				delete itemTable;
				itemTable = nullptr;
			}

			throw textureException;
		}

		Item::informationTable->put(index, itemTable);
	}

	// Set the message and redraw.
	loadProgress_->SetMessage("Item definitions successfully loaded!");
	DrawFrame();
}

void ItemManager::LoadItemsFromWeb( void )
{
	loadProgress_->AppendMessage("\n\nLoading items...");
	DrawFrame();

	uint64 userId = backpack_->GetSteamId();
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

	backpack_->LoadInventory( jsonInventory );	

	// Show success.
	loadProgress_->SetMessage("Items successfully loaded!");
	DrawFrame();

	backpack_->SetLoaded( true );
}

#include <fstream>
#include <iomanip>

void ItemManager::HandleCallbacks( void ) {
	CallbackMsg_t callback;
	if ( backpack_->GetCallback( &callback ) ) {
		switch (callback.m_iCallback) {
		case GCMessageAvailable_t::k_iCallback:
			{
				GCMessageAvailable_t *message = (GCMessageAvailable_t *)callback.m_pubParam;
				
				uint32 size;
				if ( backpack_->HasMessage( &size ) )
				{
					uint32 id, realSize = 0;

					// Retrieve the message.
					// WARNING: Do NOT use return before calling free on buffer.
					void* buffer = malloc( size );
					backpack_->GetMessage( &id, buffer, size, &realSize );

					// Filter protobuf messages.
					bool isProtobuf = (id & 0x80000000) != 0;
					uint32 realType = id & 0x0FFFFFFF;
					if (isProtobuf) {
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
							backpack_->SetTargetId( headerMsg.job_id_source() );
						}

						uint32 bodySize = size - sizeof( GCProtobufHeader_t ) - headerSize;

						switch (id & 0x0FFFFFFF) {
						case k_EMsgGCStartupCheck:
							{
								// Receive the startup check.
								CMsgStartupCheck startupMsg;
								startupMsg.ParseFromArray( headerBuffer.here(), bodySize );

								// Build a response.
								CMsgStartupCheckResponse responseMsg;
								responseMsg.set_item_schema_version( 0 );
								string responseString = responseMsg.SerializeAsString();

								// Send and free.
								backpack_->SendMessage( k_EMsgGCStartupCheckResponse | 0x80000000, (void*)responseString.c_str(), responseString.size() );
							}
							break;

						case k_EMsgGCUpdateItemSchema:
							{
								// Not handling yet.
								break;
							}

						default:
							backpack_->HandleMessage( id & 0x0FFFFFFF, headerBuffer.here(), bodySize );
							break;
						}
					}
					else {
						switch (id) {
							case GCCraftResponse_t::k_iMessage:
							{
								GCCraftResponse_t *craftMsg = (GCCraftResponse_t*)buffer;
								if (craftMsg->blueprint == 0xffff) {
									notifications_->AddNotification( "Crafting failed. No such blueprint!", nullptr );
								}

								break;
							}
						}
					}

					if (buffer != nullptr) {
						free( buffer );
						buffer = nullptr;
					}
				}
			}
		}

		backpack_->ReleaseCallback();
	} 
}

Button* ItemManager::CreateButton(const string& caption, Texture *texture, float x, float y )
{
	// Create and add.
	Button* newButton = new Button(caption, x, y);
	if (texture != nullptr) {
		newButton->SetIcon( texture );
	}

	// Add and return.
	buttonList_.push_back(newButton);
	return newButton;
}

Notice* ItemManager::CreateNotice( const string& message )
{
	Notice* newNotice = new Notice( message );
	Add( newNotice );

	// Set position.
	float x = (float)(GetWidth() / 2) - (float)(newNotice->GetWidth() / 2);
	float y = (float)(GetHeight() / 2) - (float)(newNotice->GetHeight() / 2);
	newNotice->SetPosition( x, y );
	newNotice->SetParent( this );

	// Show popup.
	ShowPopup( newNotice );

	// Add and return.
	popupList_.push_back( newNotice );
	return newNotice;
}

Alert* ItemManager::CreateAlert( const string& message )
{
	Alert* newAlert = new Alert( message );
	Add( newAlert );

	const string* str = &message;
	const char* msg = message.c_str();

	// Set position.
	float alertX = (float)(GetWidth() / 2) - (newAlert->GetWidth() / 2);
	float alertY = (float)(GetHeight() / 2) - (newAlert->GetHeight() / 2);
	newAlert->SetPosition( alertX, alertY );
	newAlert->SetParent( this );

	// Show popup.
	ShowPopup( newAlert );

	// Add and return.
	popupList_.push_back( newAlert );
	return newAlert;
}

void ItemManager::HandlePopup( Popup *popup )
{
	switch (popup->getState()) {
		case POPUP_STATE_INACTIVE:
			HidePopup(popup);
			break;
		case POPUP_STATE_KILLED:
			RemovePopup(popup);
			break;
	}
}

void ItemManager::ShowPopup( Popup* popup )
{
	popup->setState( POPUP_STATE_ACTIVE );
	popupStack_.push_back( popup );
}

void ItemManager::HidePopup( Popup *popup )
{
	// Remove popup from stack.
	deque<Popup*>::iterator popupIter;
	for (popupIter = popupStack_.begin(); popupIter != popupStack_.end(); popupIter++) {
		if (*popupIter == popup) {
			popupStack_.erase(popupIter);
			break;
		}
	}
}

void ItemManager::RemovePopup( Popup* popup )
{
	// Hide and remove.
	HidePopup( popup );
	Remove( popup );
	delete popup;
}