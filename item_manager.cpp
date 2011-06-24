#include "item_manager.h"

#include <stdlib.h>
#include <crtdbg.h>
#include <math.h>
#include <json/json.h>

#include "protobuf/base_gcmessages.pb.h"
#include "protobuf/steammessages.pb.h"
#include "protobuf/gcsdk_gcmessages.pb.h"

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#define D3D_DEBUG_INFO
#endif

using namespace std;

// Window properties.
const char*	APPLICATION_TITLE	= "Jengerer's Item Manager";
const int	APPLICATION_WIDTH	= 795;
const int	APPLICATION_HEIGHT	= 540;

// Application attributes.
const float	APPLICATION_FRAMERATE	= 30.0f;
const float APPLICATION_FRAMESPEED	= (1000.0f / APPLICATION_FRAMERATE);
const int	APPLICATION_VERSION		= 1000;

LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_DESTROY:
		PostQuitMessage( 0 );
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
	alert_ = nullptr;
	error_ = nullptr;
	backpack_ = nullptr;
	itemDisplay_ = nullptr;
	loadProgress_ = nullptr;

	// Set default running function.
	SetThink( &ItemManager::Loading );

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

	// Necessary to display progress/status.
	Notice::Precache( directX_ );
	Button::Precache( directX_ );

	try {
		// Start drawing to generate textures.
		directX_->BeginDraw();

		// Precache secondary resources.
		ItemDisplay::Precache( directX_ );
		Notification::Precache( directX_ );
		Slot::Precache( directX_ );
		ToggleSet::Precache( directX_ );

		// End drawing.
		directX_->EndDraw();

		// Create backpack.
		backpack_ = new Backpack( 0.0f, 0.0f, this );
		backpack_->Precache( directX_ );
		Add( backpack_ );

		// Create item display.
		itemDisplay_ = new ItemDisplay();
		Add( itemDisplay_ );

		// Create notification queue.
		notifications_ = new NotificationQueue();
		notifications_->SetLocalPosition( GetWidth() - BACKPACK_PADDING, GetHeight() - BACKPACK_PADDING );
		backpack_->SetNotificationQueue( notifications_ );
		Add( notifications_ );
		UpdateChild( notifications_ );

		// Create equip buttons.
		equipSet_ = nullptr;

		// Create start up message.
		loadProgress_ = CreateNotice( "Initializing item manager..." );

		// Attempt to load Steam and definitions.
		backpack_->LoadInterfaces();
		LoadDefinitions();

		// We should be good to go!
		loadProgress_->SetMessage( "Waiting for Steam inventory message..." );
		loadProgress_->CenterTo( this );
	}
	catch (Exception& loadException) {
		error_ = CreateAlert( *loadException.getMessage() );
	}
}

void ItemManager::CloseInterfaces( void )
{
	// Delete the mouse.
	if (mouse_ != nullptr) {
		delete mouse_;
		mouse_ = nullptr;
	}

	// Delete item information.
	if (Item::definitions != nullptr) {
		// Free all allocated resources.
		InformationMap::iterator i;
		for (i = Item::definitions->begin(); i != Item::definitions->end(); i++) {
			delete i->second;
		}

		// Now just remove the table.
		delete Item::definitions;
		Item::definitions = nullptr;
	}

	// Free cached resources.
	ItemDisplay::Release();
	Button::Release();
	Notice::Release();
	Notification::Release();
	Slot::Release();
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
	HandleCallbacks();
	if (backpack_->IsLoaded()) {
		SetThink( &ItemManager::Running );
		RemovePopup( loadProgress_ );
	}
}

void ItemManager::Running()
{
	HandleCallbacks();
	backpack_->HandleCamera();
	notifications_->UpdateNotifications();
	UpdateItemDisplay();
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
		// Check notification.
		if (notifications_->OnLeftClicked( mouse )) {
			return true;
		}

		// Click on backpack.
		if (backpack_->OnLeftClicked( mouse )) {
			return true;
		}
	}
	return false;
}

bool ItemManager::OnLeftReleased( Mouse *mouse )
{
	// TODO: Make popup handler with return codes like exit, close popup, etc.
	// Check top popup.
	if (!popupStack_.empty()) {
		Popup *top = popupStack_.back();
		top->OnLeftReleased( mouse );

		// Check if the popup has been closed.
		if (top == error_ && top->GetState() == POPUP_STATE_KILLED) {
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
	}

	return false;
}

bool ItemManager::OnMouseMoved( Mouse *mouse )
{
	// Pass message to highest popup.
	if (!popupStack_.empty()) {
		Popup* top = popupStack_.back();
		top->OnMouseMoved( mouse );
	}
	else {
		// Check backpack.
		if (backpack_ != nullptr && backpack_->OnMouseMoved(mouse)) {
			return true;
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
		float displayX = hovering->GetGlobalX() + hovering->GetWidth() / 2 - itemDisplay_->GetWidth() / 2;
		float displayY = hovering->GetGlobalY() + hovering->GetHeight() + ITEM_DISPLAY_SPACING;
		itemDisplay_->SetLocalPosition( displayX, displayY );
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
	loadProgress_->CenterTo( this );
	DrawFrame();

	// Load the item definitions.
	string itemDefinitions;
	try {
		itemDefinitions = directX_->read( "http://www.jengerer.com/itemManager/item_definitions.json" );
	}
	catch (Exception& ex) {
		throw Exception( "Failed to retrieve item definitions from server." );
	}

	// Begin parsing.
	Json::Reader	reader;
	Json::Value		root;
	if (!reader.parse( itemDefinitions, root, false) ) {
		throw Exception("Failed to parse item definitions.");
	}

	if (!root.isMember( "items" )) {
		throw Exception( "Unexpected definition format. Element 'items' not found." );
	}

	Json::Value		items = root.get( "items", root );
	Item::definitions = new InformationMap();
	for (Json::ValueIterator i = items.begin(); i != items.end(); ++i) {
		Json::Value thisItem = *i;
		
		bool hasKeys = thisItem.isMember("index") &&
			thisItem.isMember("name") &&
			thisItem.isMember("slot") &&
			thisItem.isMember("image");

		if (!hasKeys) {
			throw Exception("Failed to parse item definitions. One or more missing members from item entry.");
		}

		// Get strings.
		uint32 index	= thisItem.get( "index", root ).asUInt();
		string name		= thisItem.get( "name", root ).asString();
		string image	= thisItem.get( "image", root ).asString();
		EItemSlot slot	= static_cast<EItemSlot>(thisItem.get( "slot", root ).asUInt());
		uint32 classes	= thisItem.get( "classes", root ).asUInt();

		// Make sure there's a file.
		if (image.empty()) {
			image = "backpack/unknown_item";
		}

		// Attempt to load the texture.
		Texture *texture = nullptr;
		try {
			texture = directX_->GetTexture( image );
		}
		catch (Exception &textureException) {
			throw textureException;
		}

		// Generate information object.
		CItemInformation *itemInformation = new CItemInformation(
			name,
			texture,
			classes,
			slot );

		// Parse item type and insert.
		InformationPair infoPair( index, itemInformation );
		Item::definitions->insert( infoPair );
	}

	// Set the message and redraw.
	loadProgress_->SetMessage("Item definitions successfully loaded!");
	loadProgress_->CenterTo( this );
	DrawFrame();
}

void ItemManager::LoadItemsFromWeb( void )
{
	loadProgress_->AppendMessage("\n\nLoading items...");
	loadProgress_->CenterTo( this );
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
	loadProgress_->CenterTo( this );
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

Notice* ItemManager::CreateNotice( const string& message )
{
	Notice* newNotice = new Notice( message );
	Add( newNotice );

	// Set position.
	float noticeX = floor((GetWidth() - newNotice->GetWidth()) / 2.0f);
	float noticeY = floor((GetHeight() - newNotice->GetHeight())  / 2.0f);
	newNotice->SetLocalPosition( noticeX, noticeY );
	newNotice->SetParent( this );
	UpdateChild( newNotice );
	ShowPopup( newNotice );
	return newNotice;
}

Alert* ItemManager::CreateAlert( const string& message )
{
	Alert* newAlert = new Alert( message );
	Add( newAlert );

	const string* str = &message;
	const char* msg = message.c_str();

	// Set position.
	float alertX = floor((GetWidth() - newAlert->GetWidth()) / 2.0f);
	float alertY = floor((GetHeight() - newAlert->GetHeight())  / 2.0f);
	newAlert->SetGlobalPosition( alertX, alertY );
	newAlert->SetParent( this );
	UpdateChild( newAlert );
	ShowPopup( newAlert );
	return newAlert;
}

void ItemManager::CreateEquipSet( const Item *item )
{
	equipSet_ = new ToggleSet( "Equipped", "Unequipped" );
	Add( equipSet_ );
	ShowPopup( equipSet_ );

	ButtonMap::iterator i;
	for (i = equipButtons_.begin(); i != equipButtons_.end(); i++) {
		if (item->ClassUses( i->first )) {
			if (item->IsEquipped( i->first )) {
				equipSet_->AddSetA( i->second );
			}
			else {
				equipSet_->AddSetB( i->second );
			}
		}
	}

	equipSet_->Pack();
	equipSet_->SetGlobalPosition( (GetWidth() - equipSet_->GetWidth()) / 2,
		(GetHeight() - equipSet_->GetHeight()) / 2 );
}

void ItemManager::HandlePopup( Popup *popup )
{
	switch (popup->GetState()) {
		case POPUP_STATE_HIDDEN:
			HidePopup( popup );
			break;
		case POPUP_STATE_KILLED:
			RemovePopup( popup );
			break;
	}
}

void ItemManager::ShowPopup( Popup* popup )
{
	popup->SetState( POPUP_STATE_ACTIVE );
	popupStack_.push_back( popup );
}

void ItemManager::HidePopup( Popup *popup )
{
	// Remove popup from stack.
	std::vector<Popup*>::iterator popupIter = find( popupStack_.begin(), popupStack_.end(), popup );
	if (popupIter != popupStack_.end()) {
		popupStack_.erase( popupIter );
	}
}

void ItemManager::RemovePopup( Popup* popup )
{
	// Hide and remove.
	HidePopup( popup );
	Remove( popup );
	delete popup;
}