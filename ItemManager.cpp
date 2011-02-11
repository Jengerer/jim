#include "ItemManager.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <Windows.h>

using namespace std;

// Window properties.
const char*	APPLICATION_TITLE	= "Jengerer's Item Manager";
const int	APPLICATION_WIDTH	= 795;
const int	APPLICATION_HEIGHT	= 525;

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
	
	// Create menu.
	itemMenu_ = new Menu();
	itemMenu_->addOption( "Equip" );
	itemMenu_->addOption( "Craft" );
	itemMenu_->addOption( "Move" );
	itemMenu_->addOption( "Delete" );
	itemMenu_->pack();
	add( itemMenu_ );

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
	// Show dialog.
	loadDialog_ = createDialog( "Initializing item manager..." );

	try {
		// Load button textures.
		Texture *craftTexture	= getTexture( "manager/gear" );
		Texture *equipTexture	= getTexture( "manager/equip" );
		Texture *sortTexture	= getTexture( "manager/sort" );

		// Create inventory.
		backpack_ = new Backpack(
			0.0f, 0.0f, 
			PAGE_WIDTH, PAGE_HEIGHT, 
			PAGE_COUNT,
			this );
		add( backpack_ );

		// Create buttons.
		craftButton_	= createButton( "craft", craftTexture, BACKPACK_PADDING, BUTTON_Y );
		equipButton_	= createButton( "equip", equipTexture, craftButton_->getX() + craftButton_->getWidth() + BUTTON_SPACING, BUTTON_Y );
		sortButton_		= createButton( "sort", sortTexture, equipButton_->getX() + equipButton_->getWidth() + BUTTON_SPACING, BUTTON_Y );

		// Define and load items.
		loadDefinitions();
		loadItems();

		// Hide the loading dialog.
		removePopup( loadDialog_ );
		emptyTrash();
	}
	catch (Exception& loadException) {
		removePopup( loadDialog_ );
		emptyTrash();
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

	// Draw mouse position.
	stringstream mousePosition;
	mousePosition << "(" << mouse_->getX() << ", " << mouse_->getY() << ")";
	RECT screen;
	screen.top = 0;
	screen.left = BACKPACK_PADDING;
	screen.right = getWidth() - BACKPACK_PADDING;
	screen.bottom = BACKPACK_PADDING_TOP;
	drawText( mousePosition.str(), &screen, DT_SINGLELINE | DT_VCENTER, D3DCOLOR_ARGB( 200, 255, 255, 255 ) );
}

void ItemManager::run()
{
	// Handle mouse and keyboard.
	handleMouse();
	handleKeyboard();

	if (backpack_) {
		handleCallbacks();
	}

	// Redraw screen.
	redraw();
}

void ItemManager::handleMouse()
{
	// Skip if not in focus.
	if (GetFocus() == getWindow()->getHandle()) {
		// Move mouse every frame.
		mouseMoved( mouse_ );

		// Handle mouse events.
		if (keyPressed( VK_LBUTTON ) && !mouse_->isLeftDown()) {
			mouse_->setLeftMouse( true );
			leftClicked( mouse_ );
		}
		else if (!keyPressed( VK_LBUTTON ) && mouse_->isLeftDown()) {
			mouse_->setLeftMouse( false );
			leftReleased( mouse_ );
		}
		else if (keyPressed( VK_RBUTTON ) && !mouse_->isRightDown()) {
			mouse_->setRightMouse( true );
			rightClicked( mouse_ );
		}
		else if (!keyPressed( VK_RBUTTON ) && mouse_->isRightDown()) {
			mouse_->setRightMouse( false );
			rightReleased( mouse_ );
		}
	}
}

void ItemManager::handleKeyboard()
{
	// Skip if not in focus.
	if (GetFocus() == getWindow()->getHandle()) {
		// Next page on right arrow.
		if (backpack_ && backpack_->isLoaded()) {
			// Multiple item selection.
			backpack_->setSelectMode( keyPressed( VK_CONTROL ) ? SELECT_MODE_MULTIPLE : SELECT_MODE_SINGLE );

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
}

bool ItemManager::leftClicked( Mouse *mouse )
{
	// Mouse clicked.
	if (!popupStack_.empty()) {
		Popup* top = popupStack_.back();
		top->leftClicked( mouse );
		handlePopup( top );
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
		if (backpack_->leftClicked( mouse )) {
			return true;
		}
	}

	return false;
}

bool ItemManager::leftReleased( Mouse *mouse )
{
	// Check top popup.
	if (!popupStack_.empty()) {
		Popup *top = popupStack_.back();
		if (mouse->isTouching( top )) {
			top->leftReleased( mouse );
		}

		// Check if the popup has been closed.
		if (top == error_ && top->getState() == POPUP_STATE_KILLED) {
			exitApplication();
		}

		// Handle removing and hiding.
		handlePopup( top );
	}
	else {
		// Check backpack.
		if (backpack_->leftReleased( mouse )) {
			return true;
		}

		// Now run buttons.
		if (mouse->isTouching( craftButton_ )) {
			backpack_->craftSelected();
			return true;
		}
	}

	return false;
}

bool ItemManager::rightClicked( Mouse *mouse )
{
	if (!popupStack_.empty()) {
		Popup *top = popupStack_.back();
		top->rightClicked( mouse_ );
		handlePopup( top );
	}

	return true;
}

bool ItemManager::rightReleased( Mouse *mouse )
{
	if (itemMenu_->getState() != POPUP_STATE_ACTIVE) {
		showMenu( itemMenu_, mouse->getX(), mouse->getY() );
	}

	return true;
}

bool ItemManager::mouseMoved( Mouse *mouse )
{
	// Get new position.
	mouse_->pollPosition();

	// Pass message to highest popup.
	if (!popupStack_.empty()) {
		Popup* top = popupStack_.back();
		top->mouseMoved( mouse );
	}
	else {
		// Check backpack.
		if (backpack_ && backpack_->mouseMoved( mouse )) {
			return true;
		}

		// Check all buttons.
		vector<Button*>::iterator i;
		bool hitButton = false;
		for (i = buttonList_.begin(); i != buttonList_.end(); i++) {
			Button *button = *i;
			if (button->mouseMoved( mouse )) {
				hitButton = true;
			}
		}

		if (hitButton) {
			return true;
		}
	}

	return false;
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

	backpack_->setLoaded();
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

					// Retrieve the message.
					void* buffer = malloc( size );
					backpack_->getMessage( &id, buffer, size, &realSize );

					switch (id) {
					case SOMsgCacheSubscribed_t::k_iMessage:
						// Item information has been received.
						{
							// Start loading items.
							SerializedBuffer serializedBuffer(buffer);
							SOMsgCacheSubscribed_t *list = serializedBuffer.get<SOMsgCacheSubscribed_t>();

							// Don't load if we've already loaded.
							if (!backpack_->isLoaded() || (list->steamid != backpack_->getSteamId())) {
								for (int i = 0; i < list->itemcount; i++)
								{
									SOMsgCacheSubscribed_Item_t *item = serializedBuffer.get<SOMsgCacheSubscribed_Item_t>();

									// Get custom item name.
									char* customName	= 0;
									if (item->namelength > 0) {
										customName = (char*)serializedBuffer.here();
										serializedBuffer.push( item->namelength );
									}

									serializedBuffer.push( sizeof( uint8 ) ); // Skip first unknown.
									uint8* origin		= serializedBuffer.get<uint8>();
									uint16* descLength	= serializedBuffer.get<uint16>();

									// Get custom description.
									char* customDesc	= 0;
									if (*descLength > 0) {
										customDesc = (char*)serializedBuffer.here();
										serializedBuffer.push( *descLength );
									}

									serializedBuffer.push( sizeof( uint8 ) ); // Skip second unknown.
									uint16* attribCount	= serializedBuffer.get<uint16>();

									// Skip past attributes.
									serializedBuffer.push( sizeof(SOMsgCacheSubscribed_Item_Attrib_t) * (*attribCount) );

									// Push two 32-bit ints.
									serializedBuffer.push( sizeof(uint64) );

									//Create a new item from the information.
									Item *newItem = new Item(
										item->itemid,
										item->itemdefindex,
										item->itemlevel,
										(EItemQuality)item->itemquality,
										item->itemcount,
										item->position );

									// Add the item.
									backpack_->addItem( newItem );
								}
							}

							backpack_->setLoaded();
							backpack_->updateExcluded();
							break;
						}

					case GCCraftResponse_t::k_iMessage:
						{
							GCCraftResponse_t *pResponse = (GCCraftResponse_t*)buffer;

							if (pResponse->blueprint == 0xffff) {
								createAlert( "Not sure what you were trying to do there, but crafting failed. No such blueprint exists!" );
							}
							else {
								createAlert( "Congratulations! Item crafting succeeded!" );
							}

							break;
						}
					case SOMsgCreate_t::k_iMessage:
						{
							SOMsgCreate_t *msgCreate = (SOMsgCreate_t*)buffer;
							SOMsgCacheSubscribed_Item_t* craftedItem = &msgCreate->item;

							//Make sure it's a valid item.
							if (msgCreate->unknown == 5) {
								break;
							}

							Item* newItem = new Item(
								craftedItem->itemid,
								craftedItem->itemdefindex,
								craftedItem->itemlevel,
								(EItemQuality)craftedItem->itemquality,
								craftedItem->itemcount,
								craftedItem->position);

							// Add this item to excluded.
							backpack_->addItem( newItem );
							backpack_->updateExcluded();

							break;
						}
					case SOMsgDeleted_t::k_iMessage:
						{
							SOMsgDeleted_t *pDeleted = (SOMsgDeleted_t*)buffer;
							backpack_->removeItem( pDeleted->itemid );
							backpack_->updateExcluded();

							break;
						}
					}

					// Free up memory.
					if (buffer)
						free(buffer);
				}
				
			}
		}

		backpack_->releaseCallback();
	}
}

Button* ItemManager::createButton( const string& caption, Texture *texture, float x, float y, EAlignment align )
{
	// Create and add.
	Button* newButton = new Button( caption, texture, x, y, align );
	add( newButton );

	// Add and return.
	buttonList_.push_back( newButton );
	return newButton;
}

Dialog* ItemManager::createDialog( const string& message )
{
	Dialog* newDialog = new Dialog( message );
	add( newDialog );

	// Set position.
	float x = (float)(getWidth() / 2) - (float)(newDialog->getWidth() / 2);
	float y = (float)(getHeight() / 2) - (float)(newDialog->getHeight() / 2);
	newDialog->setPosition( x, y );
	newDialog->setParent( this );

	// Show popup.
	showPopup( newDialog );

	// Add and return.
	popupList_.push_back( newDialog );
	return newDialog;
}

Alert* ItemManager::createAlert( const string& message )
{
	Alert* newAlert = new Alert( message );
	add( newAlert );

	const string* str = &message;
	const char* msg = message.c_str();

	// Set position.
	float alertX = (float)(getWidth() / 2) - (newAlert->getWidth() / 2);
	float alertY = (float)(getHeight() / 2) - (newAlert->getHeight() / 2);
	newAlert->setPosition( alertX, alertY );
	newAlert->setParent( this );

	// Show popup.
	showPopup( newAlert );

	// Add and return.
	popupList_.push_back(newAlert);
	return newAlert;
}

void ItemManager::showMenu( Menu *menu, int x, int y )
{
	// Position so that menus don't exceed bounds.
	if (x + menu->getWidth() > getWidth()) {
		x -= menu->getWidth();
	}

	if (y + menu->getHeight() > getHeight()) {
		y -= menu->getHeight();
	}

	menu->setPosition( x, y );
	showPopup( menu );
}



void ItemManager::handlePopup( Popup *popup )
{
	switch (popup->getState()) {
		case POPUP_STATE_INACTIVE:
			hidePopup( popup );
			break;
		case POPUP_STATE_KILLED:
			removePopup( popup );
			break;
	}
}

void ItemManager::showPopup( Popup* popup )
{
	popup->setState( POPUP_STATE_ACTIVE );
	popupStack_.push_back( popup );
}

void ItemManager::hidePopup( Popup *popup )
{
	// Remove popup from stack.
	deque<Popup*>::iterator popupIter;
	for (popupIter = popupStack_.begin(); popupIter != popupStack_.end(); popupIter++) {
		if (*popupIter == popup) {
			popupStack_.erase( popupIter );
			break;
		}
	}
}

void ItemManager::removePopup( Popup* popup )
{
	// Hide and remove.
	hidePopup( popup );
	trash( popup );
}