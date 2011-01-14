#include "ItemManager.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <Windows.h>

using namespace std;

// Window properties.
const char*	APPLICATION_TITLE	= "Jengerer's Item Manager";
const int	APPLICATION_WIDTH	= 795;
const int	APPLICATION_HEIGHT	= 500;

// Application attributes.
const float	APPLICATION_FRAMERATE	= 30.0f;
const float APPLICATION_FRAMESPEED	= (1000.0f / APPLICATION_FRAMERATE);
const int	APPLICATION_VERSION		= 1000;

// Inventory attributes.
const int	PAGE_WIDTH			= 10;
const int	PAGE_HEIGHT			= 5;
const int	PAGE_COUNT			= 6;

LRESULT CALLBACK wndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage( 0 );
		break;
	}

	return DefWindowProc( hWnd, message, wParam, lParam );
}

int WINAPI WinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	ItemManager* itemManager = 0;

	try {
		itemManager = new ItemManager(hInstance);
		itemManager->openInterfaces();
	}
	catch (Exception mainException) {
		MessageBox( NULL, mainException.getMessage()->c_str(), "Initialization failed!", MB_OK );
		return EXIT_FAILURE;
	}

	bool isDone = false;

	MSG msg;
	while (!isDone) {
		if (PeekMessage( &msg, NULL, 0, 0, PM_REMOVE )) {
			if (msg.message == WM_QUIT)
				isDone = true;

			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}

		itemManager->run();
	}

	if (itemManager) {
		delete itemManager;
		itemManager = 0;
	}

	_CrtDumpMemoryLeaks();

	return EXIT_SUCCESS;
}

ItemManager::ItemManager( HINSTANCE hInstance ): DirectX( APPLICATION_TITLE, hInstance, APPLICATION_WIDTH, APPLICATION_HEIGHT )
{
	// Zero all pointers.
	backpack_ = 0;
	alert_ = error_ = 0;

	// Create mouse.
	mouse_ = new Mouse( getWindow() );
	leftPressed_ = rightPressed_ 
		= enterPressed_ 
		= false;
}

ItemManager::~ItemManager()
{
	// ItemManager has been destroyed.
	closeInterfaces();
}

void ItemManager::openInterfaces()
{
	// Load the UI textures.
	Slot::texture		= getTexture( "manager/item_slot" );
	Button::texture		= getTexture( "manager/button" );
	Dialog::texture		= getTexture( "manager/dialog_box" );

	// Create buttons.
	deleteButton_	= createButton( "delete",	650.0f,	355.0f );
	craftButton_	= createButton( "craft",	520.0f,	355.0f );
	sortButton_		= createButton( "sort",		25.0f,	355.0f );
	exitButton_		= createButton( "exit",		650.0f,	425.0f );

	// Show dialog.
	loadDialog_ = createDialog( "Initializing..." );

	try {
		// Create inventory.
		backpack_ = new Backpack(
			0.0f, 0.0f, 
			PAGE_WIDTH, PAGE_HEIGHT, 
			PAGE_COUNT,
			this );
		add( backpack_ );

		// Define and load items.
		loadDefinitions();
		loadItems();

		// Set backpack to loaded.
		backpack_->setLoaded();

		// Hide the loading dialog.
		removePopup( loadDialog_ );
		alert_ = createAlert( "Everything was loaded successfully!" );
	}
	catch (Exception& loadException) {
		removePopup( loadDialog_ );
		error_ = createAlert( *loadException.getMessage() );
	}
}

void ItemManager::closeInterfaces()
{
	// Delete the mouse.
	if (mouse_) {
		delete mouse_;
		mouse_ = 0;
	}

	// Delete item information.
	if (Item::informationTable) {
		delete Item::informationTable;
		Item::informationTable = 0;
	}
}

DWORD lastTick = 0;

// TODO: Set slot position only once, and modify with camera view (maybe use translate).
void ItemManager::onRedraw()
{
	if (backpack_ && backpack_->isLoaded()) {
		// Draw buttons.
		vector<Button*>::iterator buttonIter;
		for (buttonIter = buttonList_.begin(); buttonIter != buttonList_.end(); buttonIter++) {
			Button* button = *buttonIter;
			button->draw( this );
		}

		// Draw backpack.
		backpack_->draw( this );
	}

	// Draw dialog boxes.
	deque<Popup*>::iterator popupIter;
	for (popupIter = popupStack_.begin(); popupIter != popupStack_.end(); popupIter++) {
		Popup* popup = *popupIter;
		popup->draw( this );
	}

	DWORD cur = GetTickCount();
	DWORD diff = cur - lastTick;
	lastTick = cur;
	float fps = 1000 / (float)diff;

	// Draw mouse position.
	stringstream mousePosition;
	mousePosition << "(" << mouse_->getX() << ", " << mouse_->getY() << ") at " << (int)fps << "FPS";
	RECT screen;
	screen.top = 0;
	screen.left = 5;
	screen.right = getWidth() - 5;
	screen.bottom = 25;
	drawText( mousePosition.str(), &screen, DT_SINGLELINE | DT_VCENTER, D3DCOLOR_ARGB( 200, 255, 255, 255 ) );
}

void ItemManager::run()
{
	// Handle mouse and keyboard.
	handleMouse();
	handleKeyboard();

	// Redraw screen.
	redraw();
}

void ItemManager::handleMouse()
{
	// Move mouse every frame.
	triggerMouse( MOUSE_EVENT_MOVE );

	if (keyPressed( VK_LBUTTON ) && !mouse_->isLeftDown()) {
		mouse_->leftMouseDown();
		triggerMouse( MOUSE_EVENT_CLICK );
	}
	else if (!keyPressed( VK_LBUTTON ) && mouse_->isLeftDown()) {
		mouse_->leftMouseUp();
		triggerMouse( MOUSE_EVENT_RELEASE );
	}
}

void ItemManager::handleKeyboard()
{
	// Next page on right arrow.
	if (backpack_ && backpack_->isLoaded()) {
		if (keyPressed( VK_RIGHT )) {
			if (!rightPressed_) {
				backpack_->nextPage();
				rightPressed_ = true;
			}
		}
		else {
			rightPressed_ = false;
		}

		// Previous page on left arrow.
		if (keyPressed( VK_LEFT )) {
			if (!leftPressed_) {
				backpack_->prevPage();
				leftPressed_ = true;
			}
		}
		else {
			leftPressed_ = false;
		}
	}

	// Close dialogs on enter.
	if (keyPressed( VK_RETURN )) {
		if (!enterPressed_) {
			if (!popupStack_.empty()) {
				Popup* popup = popupStack_.back();

				// Exit if error.
				if (popup == error_) {
					exitApplication();
				}

				removePopup( popup );
			}
			enterPressed_ = true;
		}
	}
	else {
		enterPressed_ = false;
	}

	// Exit program on escape.
	if (keyPressed( VK_ESCAPE )) {
		exitApplication();
	}
}

void ItemManager::triggerMouse( EMouseEvent eventType )
{
	// Update position.
	if (eventType == MOUSE_EVENT_MOVE) {
		mouse_->pollPosition();
	}

	// Send message.
	mouse_->triggerEvent( this, eventType );
}

void ItemManager::mouseClicked( Mouse *mouse, Component *component )
{
	// Mouse clicked.
	if (!popupStack_.empty()) {
		Popup* top = popupStack_.back();
		if (top == component) {
			// Starting dragging top components.
			top->onDrag( mouse, this );
		}
	}
}

void ItemManager::mouseReleased( Mouse *mouse, Component *component )
{
	// Mouse released.
	if (!popupStack_.empty()) {
		Popup* top = popupStack_.back();
		if (top == component) {
			// Handle button if this is an alert.
			if (top == alert_) {
				const Button *alertButton = alert_->getButton();

				if (mouse->isTouching( alertButton )) {
					removePopup( alert_ );
				}
				else {
					alert_->onRelease();
				}
			}
			else if (top == error_) {
				const Button *errorButton = error_->getButton();

				if (mouse->isTouching( errorButton )) {
					removePopup( error_ );
					exitApplication();
				}
				else {
					error_->onRelease();
				}
			}
		}
		return;
	}
}

void ItemManager::mouseMoved( Mouse *mouse, Component *component )
{
	// Get new position.
	mouse_->pollPosition();

	// Update popup position.
	if (popupStack_.size()) {
		Popup* top = popupStack_.back();
		top->updatePosition();
	}
}

void ItemManager::buttonPressed( Button* button )
{
	// Button pressed.
}

void ItemManager::buttonReleased( Button* button )
{
	// Button released.
}

void ItemManager::loadDefinitions()
{
	// Set the message and redraw.
	loadDialog_->setMessage( "Loading item definitions..." );
	redraw();

	// Load the item definitions.
	string itemDefinitions = read( "http://www.jengerer.com/itemManager/itemDefinitions.json" );

	// Begin parsing.
	Json::Reader	reader;
	Json::Value		root;
	if (!reader.parse( itemDefinitions, root, false ))
		throw Exception( "Failed to parse item definitions." );

	unsigned int texturesLoaded = 0;
	unsigned int textureCount = root.size();
	Item::informationTable = new Hashtable();
	for (Json::ValueIterator i = root.begin(); i != root.end(); i++) {
		Json::Value thisItem = *i;
		
		bool hasKeys = thisItem.isMember("index") &&
			thisItem.isMember("name") &&
			thisItem.isMember("slot") &&
			thisItem.isMember("image");

		if (!hasKeys)
			throw Exception( "Failed to parse item definitions. One or more missing members from item entry." );

		// Get strings.
		string index	= thisItem.get( "index", root ).asString();
		string name		= thisItem.get( "name",	root ).asString();
		string image	= thisItem.get( "image", root ).asString();
		string slot		= thisItem.get( "slot",	root ).asString();

		// Make sure there's a file.
		if (image.length() == 0)
			image = "backpack/unknown_item";

		// Add strings to new table.
		Hashtable* itemTable = new Hashtable();
		itemTable->put("name", new string(name));
		itemTable->put("image", new string(image));
		itemTable->put("slot", new string(slot));

		try {
			// Get the texture, add to table.
			Texture* texture = getTexture( image );
			itemTable->put( "texture", texture );
		}
		catch (Exception &textureException) {
			if (itemTable) {
				delete itemTable;
				itemTable = 0;
			}

			throw textureException;
		}

		Item::informationTable->put( index, itemTable );
	}

	// Set the message and redraw.
	loadDialog_->setMessage( "Item definitions successfully loaded!" );
	redraw();
}

void ItemManager::loadItems()
{
	// Append the message.
	loadDialog_->appendMessage( "\n\nLoading items..." );
	redraw();

	// First clear items.
	backpack_->clearItems();

	/* Get user's Steam community URL. */
	uint64 userId = backpack_->getSteamId();

	stringstream urlStream;
	urlStream << "http://api.steampowered.com/ITFItems_440/GetPlayerItems/v0001/?key=0270F315C25E569307FEBDB67A497A2E&SteamID=" << userId << "&format=json";
	string apiUrl = urlStream.str();

	// Attempt to read the file.
	string jsonInventory;
	try {
		jsonInventory = read( apiUrl );
	}
	catch (Exception curlException) {
		throw Exception( "Failed to read inventory from profile." );
	}
	
	// Begin parsing the string.
	Json::Reader	reader;
	Json::Value		root;

	// Attempt to parse it.
	if (!reader.parse( jsonInventory, root, false ))
		throw Exception( "Failed to parse inventory JSON file." );

	// Get the result.
	if (!root.isMember( "result" ))
		throw Exception( "Failed to parse player's items from Web API: no 'result' key received." );

	Json::Value result = root.get( "result", root );

	// Get the JSON value for the status.
	if (!result.isMember( "status" ))
		throw Exception( "Failed to parse player's items from Web API: no 'status' key received." );
	int status = result.get( "status", root ).asInt();

	// Check validity.
	if (status == 15)
		throw Exception( "Failed to parse player's items from Web API: profile is private." );
	else if (status == 8)
		throw Exception( "Failed to parse player's items from Web API: invalid SteamID argument." );

	// Get the items object.
	if (!result.isMember( "items" ))
		throw Exception( "Failed to parse player's items from Web API: no 'items' key received." );
	Json::Value items = result.get( "items", root );
	
	// TODO: Check that 'item' exists in an empty inventory.
	// Get items array.
	if (!items.isMember( "item" ))
		throw Exception( "Failed to parse player's items from Web API: no 'item' key received." );
	Json::Value itemsArray = items.get( "item", root );
	
	// Keep track of attributes.
	SOMsgCacheSubscribed_Item_t item;
	ZeroMemory( &item, sizeof( SOMsgCacheSubscribed_Item_t ));

	for (Json::ValueIterator i = itemsArray.begin(); i != itemsArray.end(); i++) {
		Json::Value thisItem = *i;

		bool hasKeys = thisItem.isMember( "id" ) && 
			thisItem.isMember( "defindex" ) && 
			thisItem.isMember( "level" ) && 
			thisItem.isMember( "quality" ) && 
			thisItem.isMember( "inventory" ) &&
			thisItem.isMember( "quantity" );

		if (!hasKeys)
			throw Exception( "Failed to parse player's items from Web API: unexpected format for items received." );

		item.itemid			= thisItem.get( "id", root ).asUInt64();
		item.itemdefindex	= thisItem.get( "defindex", root ).asUInt();
		item.itemlevel		= thisItem.get( "level", root ).asUInt();
		item.itemquality	= thisItem.get( "quality", root ).asUInt();
		item.position		= thisItem.get( "inventory", root ).asUInt();

		// All attributes retrieved, make item.
		Item* newItem = new Item(
			item.itemid,
			item.itemdefindex,
			item.itemlevel,
			(EItemQuality)item.itemquality,
			item.itemcount,
			item.position );

		// Add the item.
		backpack_->addItem( newItem );
	}

	// Show success.
	loadDialog_->setMessage( "Items successfully loaded!" );
	redraw();

	//TODO: Update excluded scrolling.
}

void ItemManager::handleCallbacks() {
	//Get waiting callback.
	CallbackMsg_t callback;
	if (backpack_->getCallback( &callback ))
	{
		switch (callback.m_iCallback)
		{
		case GCMessageAvailable_t::k_iCallback:
			{
				GCMessageAvailable_t *message = (GCMessageAvailable_t *)callback.m_pubParam;
				
				uint32 size;
				if (backpack_->hasMessage( &size ))
				{
					unsigned int id, realSize = 0;

					// Allocate memory.
					void* buffer = malloc( size );

					// Retrieve the message.
					backpack_->getMessage( &id, buffer, size, &realSize );

					switch (id)
					{
					case SOMsgCacheSubscribed_t::k_iMessage:
						// Item information has been received.
						{
							// Start loading items.
							SerializedBuffer serializedBuffer(buffer);
							SOMsgCacheSubscribed_t *iList = serializedBuffer.get<SOMsgCacheSubscribed_t>();
							for (int I = 0; I < iList->itemcount; I++)
							{
								SOMsgCacheSubscribed_Item_t *pItem = serializedBuffer.get<SOMsgCacheSubscribed_Item_t>();

								// Skip past the name.
								serializedBuffer.push( pItem->namelength );

								// Get attribute count, and skip past.
								uint16* attribCount = serializedBuffer.get<uint16>();
								serializedBuffer.push<SOMsgCacheSubscribed_Item_Attrib_t>( *attribCount );

								//Create a new item from the information.
								Item *newItem = new Item(
									pItem->itemid,
									pItem->itemdefindex,
									pItem->itemlevel,
									(EItemQuality)pItem->itemquality,
									pItem->itemcount,
									pItem->position );

								// Add the item.
								backpack_->add( newItem );
							}

							//TODO: Update scrolling of excluded items
							break;
						}

					case GCCraftResponse_t::k_iMessage:
						{
							GCCraftResponse_t *pResponse = (GCCraftResponse_t*)buffer;

							if (pResponse->blueprint == 0xFFFF) {
								//TODO: Display the dialog for crafting failed.
							}

							break;
						}
					case SOMsgCreate_t::k_iMessage:
						{
							SOMsgCreate_t *msgCreate = (SOMsgCreate_t*)buffer;
							SOMsgCacheSubscribed_Item_t* craftedItem = &msgCreate->item;

							//Make sure it's a valid item.
							if (msgCreate->unknown == 5)
								break;

							Item* newItem = new Item(
								craftedItem->itemid,
								craftedItem->itemdefindex,
								craftedItem->itemlevel,
								(EItemQuality)craftedItem->itemquality,
								craftedItem->itemcount,
								craftedItem->position);

							// Add this item to excluded.
							backpack_->addItem( newItem );

							//TODO: Update excluded scrolling.

							break;
						}
					case SOMsgDeleted_t::k_iMessage:
						{
							SOMsgDeleted_t *pDeleted = (SOMsgDeleted_t*)buffer;

							// TODO: Iterate through both vectors and remove it.

							break;
						}
					}

					// Free up memory.
					if (buffer)
						free(buffer);
				}
				
			}
		}
	}

	backpack_->releaseCallback();
}

Button* ItemManager::createButton( const string& caption, float x, float y )
{
	Button* newButton = new Button( caption, x, y );
	add( newButton );

	// Add and return.
	buttonList_.push_back( newButton );
	return newButton;
}

Dialog* ItemManager::createDialog( const string& message )
{
	Dialog* newDialog = new Dialog( message );
	newDialog->setMouseListener( this );
	add( newDialog );

	// Set position.
	float x = (float)(getWidth() / 2) - (float)(newDialog->getWidth() / 2);
	float y = (float)(getHeight() / 2) - (float)(newDialog->getHeight() / 2);
	newDialog->setPosition( x, y );

	// Show popup.
	showPopup( newDialog );

	// Add and return.
	popupList_.push_back( newDialog );
	return newDialog;
}

Alert* ItemManager::createAlert( const string& message )
{
	Alert* newAlert = new Alert( message );
	newAlert->setMouseListener( this );
	add( newAlert );

	const string* str = &message;
	const char* msg = message.c_str();

	// Set position.
	float alertX = (float)(getWidth() / 2) - (newAlert->getWidth() / 2);
	float alertY = (float)(getHeight() / 2) - (newAlert->getHeight() / 2);
	newAlert->setPosition( alertX, alertY );

	// Show popup.
	showPopup( newAlert );

	// Add and return.
	popupList_.push_back(newAlert);
	return newAlert;
}

void ItemManager::showPopup( Popup* whichPopup )
{
	popupStack_.push_back( whichPopup );
}

void ItemManager::removePopup( Popup* whichPopup )
{
	// Hide and remove the notification.
	deque<Popup*>::iterator popupIter;
	for (popupIter = popupStack_.begin(); popupIter != popupStack_.end(); popupIter++) {
		if (*popupIter == whichPopup) {
			popupStack_.erase( popupIter );
			break;
		}
	}

	// Now remove from container.
	remove( whichPopup );
	delete whichPopup;
}