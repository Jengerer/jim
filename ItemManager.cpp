#include "ItemManager.h"

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#endif

#include <stdlib.h>
#include <crtdbg.h>

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
	switch (message) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	ItemManager* itemManager = nullptr;

	try {
		itemManager = new ItemManager(hInstance);
		itemManager->openInterfaces();
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

		itemManager->run();
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

ItemManager::ItemManager(HINSTANCE instance) : Application(APPLICATION_TITLE, instance, APPLICATION_WIDTH, APPLICATION_HEIGHT)
{
	alert_ = 0;
	error_ = 0;
	backpack_ = 0;
	mouse_ = new Mouse(directX_);

	listenKey(VK_LEFT);
	listenKey(VK_RIGHT);
	listenKey(VK_ESCAPE);
	listenKey(VK_RETURN);
	listenKey(VK_LCONTROL);
}

ItemManager::~ItemManager()
{
	// ItemManager has been destroyed.
	closeInterfaces();
}

void ItemManager::openInterfaces()
{
	// Show dialog.
	loadDialog_ = createDialog("Initializing item manager...");

	try {
		// Create buttons.
		Texture *craftTexture = directX_->getTexture("manager/gear");
		Texture *equipTexture = directX_->getTexture("manager/equip");
		Texture *sortTexture = directX_->getTexture("manager/sort");

		// Add all buttons to a layout.
		HorizontalLayout *buttonLayout = new HorizontalLayout();
		buttonLayout->setSpacing(BUTTON_SPACING);
		buttonLayout->setPosition(BACKPACK_PADDING, BUTTON_Y);
		craftButton_	= createButton("craft", craftTexture, BACKPACK_PADDING, BUTTON_Y);
		equipButton_	= createButton("equip", equipTexture, craftButton_->getX() + craftButton_->getWidth() + BUTTON_SPACING, BUTTON_Y);
		sortButton_		= createButton("sort", sortTexture, equipButton_->getX() + equipButton_->getWidth() + BUTTON_SPACING, BUTTON_Y);
		buttonLayout->add(craftButton_);
		buttonLayout->add(equipButton_);
		buttonLayout->add(sortButton_);
		buttonLayout->pack();

		backpack_ = new Backpack(0.0f, 0.0f, this);
		backpack_->createInventory(PAGE_WIDTH, PAGE_HEIGHT, PAGE_COUNT, EXCLUDED_SIZE);
		addBottom(backpack_);
		addBottom(buttonLayout);
		backpack_->openInterfaces();
		loadDefinitions();
		loadItems();

		// Set default state.
		equipButton_->disable();
		craftButton_->disable();
		sortButton_->disable();

		// Hide the loading dialog.
		removePopup(loadDialog_);

		// Enable sorting.
		sortButton_->enable();

		// Set application ready to run.
		setState(APPLICATION_STATE_RUN);
	}
	catch (Exception& loadException) {
		removePopup(loadDialog_);
		error_ = createAlert(*loadException.getMessage());
		setState(APPLICATION_STATE_EXIT);
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

void ItemManager::run()
{
	updateKeys();
	if (GetFocus() == getWindow()->getHandle()) {
		handleMouse();
		handleKeyboard();
	}

	if (getState() == APPLICATION_STATE_RUN) {
		handleCallbacks();
		backpack_->handleCamera();
	}

	// Redraw screen.
	redraw();
}

bool ItemManager::leftClicked(Mouse *mouse)
{
	// Mouse clicked.
	if (!popupStack_.empty()) {
		Popup* top = popupStack_.back();
		top->leftClicked(mouse);
		handlePopup(top);
		return true;
	}
	else {
		// Check, but don't register buttons.
		vector<Button*>::iterator i;
		for (i = buttonList_.begin(); i != buttonList_.end(); i++) {
			if (mouse->isTouching(*i)) {
				return true;
			}
		}

		// Check backpack.
		if (backpack_->leftClicked(mouse)) {
			slotVector* selected = backpack_->getSelected();

			// Set equip button state.
			if (selected->size() == 1) {
				Slot *slot = selected->at(0);
				Item *item = slot->getItem();
				Hashtable *classes = item->getClasses();
				if (classes != 0) {
					equipButton_->enable();
				}
				else {
					equipButton_->disable();
				}
			}
			else {
				equipButton_->disable();
			}

			// Set craft button state.
			if (selected->size() != 0) {
				craftButton_->enable();
			}
			else {
				craftButton_->disable();
			}

			return true;
		}

		craftButton_->disable();
		equipButton_->disable();
	}
	return false;
}

bool ItemManager::leftReleased(Mouse *mouse)
{
	// Check top popup.
	if (!popupStack_.empty()) {
		Popup *top = popupStack_.back();
		if (mouse->isTouching(top)) {
			top->leftReleased(mouse);
		}

		// Check if the popup has been closed.
		if (top == error_ && top->getState() == POPUP_STATE_KILLED) {
			exitApplication();
		}

		// Handle removing and hiding.
		handlePopup(top);
	}
	else {
		// Check backpack.
		if (backpack_->leftReleased(mouse)) {
			return true;
		}

		// Now run buttons.
		if (craftButton_->isEnabled() && mouse->isTouching(craftButton_)) {
			backpack_->craftSelected();
			return true;
		}
		else if (equipButton_->isEnabled() && mouse->isTouching(equipButton_)) {
			slotVector* selected = backpack_->getSelected();
			Slot* slot = selected->at(0);
			Item* item = slot->getItem();
			Hashtable* classes = item->getClasses();
			if (classes->size() > 1) {
				// Show equip menu.
				
			}
			else if (classes->size() == 1) {
				// Get the class.
				string className = classes->begin()->first;
				// TODO: Use some integer or enum to handle classes, not strings.
				backpack_->equipItem(item, className);
			}
		}
	}

	return false;
}

bool ItemManager::mouseMoved(Mouse *mouse)
{
	// Get new position.
	mouse_->pollPosition();
	SetCursor(arrow_);

	// Pass message to highest popup.
	if (!popupStack_.empty()) {
		Popup* top = popupStack_.back();
		top->mouseMoved(mouse);
	}
	else {
		// Check backpack.
		if (backpack_ && backpack_->mouseMoved(mouse)) {
			if (backpack_->getHovered() != 0) {
				SetCursor(hand_);
			}

			return true;
		}

		// Check all buttons.
		vector<Button*>::iterator i;
		bool hitButton = false;
		for (i = buttonList_.begin(); i != buttonList_.end(); i++) {
			Button *button = *i;
			if (button->isEnabled() && button->mouseMoved(mouse)) {
				SetCursor(hand_);
				hitButton = true;
			}
		}

		if (hitButton) {
			return true;
		}
	}

	return false;
}

void ItemManager::handleKeyboard()
{
	if (!popupStack_.empty()) {
		Popup *top = popupStack_.back();
		if (isPressed(VK_RETURN)) {
			if (top == error_) {
				exitApplication();
			}
			removePopup(top);
		}
	}
	else {
		if (isPressed(VK_ESCAPE)) {
			exitApplication();
		}
		else {
			if (backpack_ && backpack_->isLoaded()) {
				// Toggle between single and multiple selection.
				backpack_->setSelectMode(isPressed(VK_LCONTROL) ? SELECT_MODE_MULTIPLE : SELECT_MODE_SINGLE);

				if (isClicked(VK_LEFT)) {
					backpack_->prevPage();
				}
				else if (isClicked(VK_RIGHT)) {
					backpack_->nextPage();
				}
			}
		}
	}
}

void ItemManager::loadDefinitions()
{
	// Set the message and redraw.
	loadDialog_->setMessage("Loading item definitions...");
	redraw();

	// Load the item definitions.
	string itemDefinitions = directX_->read("http://www.jengerer.com/itemManager/itemDefinitions.json");

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
		string index	= thisItem.get("index", root).asString();
		string name		= thisItem.get("name",	root).asString();
		string image	= thisItem.get("image", root).asString();
		string slot		= thisItem.get("slot",	root).asString();

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
			Texture* texture = directX_->getTexture(image);
			itemTable->put("texture", texture);
		}
		catch (Exception &textureException) {
			if (itemTable) {
				delete itemTable;
				itemTable = 0;
			}

			throw textureException;
		}

		Item::informationTable->put(index, itemTable);
	}

	// Set the message and redraw.
	loadDialog_->setMessage("Item definitions successfully loaded!");
	redraw();
}

void ItemManager::loadItems()
{
	loadDialog_->appendMessage("\n\nLoading items...");
	redraw();
	uint64 userId = backpack_->getSteamId();

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

	backpack_->loadInventory(jsonInventory);	

	// Show success.
	loadDialog_->setMessage("Items successfully loaded!");
	redraw();

	backpack_->setLoaded(true);
}

#include <fstream>
#include <iomanip>

void ItemManager::handleCallbacks() {
	CallbackMsg_t callback;
	if (backpack_->getCallback(&callback)) {
		switch (callback.m_iCallback) {
		case GCMessageAvailable_t::k_iCallback:
			{
				GCMessageAvailable_t *message = (GCMessageAvailable_t *)callback.m_pubParam;
				
				uint32 size;
				if (backpack_->hasMessage(&size))
				{
					uint32 id, realSize = 0;

					// Retrieve the message.
					void* buffer = malloc(size);
					backpack_->getMessage(&id, buffer, size, &realSize);

#ifdef _DEBUG
					ofstream file("output.txt", ios_base::app);
					file << "type: " << hex << id << endl;
					for (int i = 0; i < size; i++) {
						file << hex << setw(2) << setfill('0') << (unsigned int)((unsigned char*)buffer)[i] << " ";
					}
					file << endl << endl;
					file.close();
#endif
					// Filter protobuf messages.
					if (id & 0x80000000 != 0) {
						// First get the protobuf struct header.
						SerializedBuffer headerBuffer(buffer);
						GCProtobufHeader_t *headerStruct = headerBuffer.get<GCProtobufHeader_t>();
						uint32 headerSize = headerStruct->m_cubProtobufHeader;

						// Now get the real protobuf header.
						CMsgProtoBufHeader headerMsg;
						void *headerBytes = headerBuffer.here();
						headerMsg.ParseFromArray( headerBytes, headerSize );
						headerBuffer.push( headerSize );

						switch (id & 0x0FFFFFFF) {
						case k_EMsgGCStartupCheck:
							{
								CMsgStartupCheck startupMsg;
								startupMsg.ParseFromArray( headerBuffer.here(), size - sizeof( GCProtobufHeader_t ) - headerSize );
								CMsgStartupCheckResponse responseMsg;
								responseMsg.set_item_schema_version(0);
								string responseString = responseMsg.SerializeAsString();

								// Create header for response.
								CMsgProtoBufHeader responseHeader;
								responseHeader.set_client_steam_id( headerMsg.client_steam_id() );
								responseHeader.set_job_id_target( headerMsg.job_id_source() );
								string headerData = responseHeader.SerializeAsString();

								// Fill in struct.
								GCProtobufHeader_t *responseStruct = new GCProtobufHeader_t;
								responseStruct->m_cubProtobufHeader = headerData.length();
								responseStruct->m_EMsg = k_EMsgGCStartupCheckResponse | 0x80000000;

								// Append messages.
								uint32 responseSize = sizeof( GCProtobufHeader_t ) + headerData.length() + responseString.length();
								void* response = malloc( responseSize );
								SerializedBuffer responseBuffer( response );
								responseBuffer.write( responseStruct, sizeof( GCProtobufHeader_t ) );
								responseBuffer.write( (void*)headerData.c_str(), headerData.length() );
								responseBuffer.write( (void*)responseString.c_str(), responseString.length() );

#ifdef _DEBUG
								ofstream responseOut( "response.txt" );
								unsigned char* responseChar = (unsigned char*)responseBuffer.start();
								for (int i = 0; i < responseSize; i++) {
									responseOut << hex << setw(2) << setfill('0') << (unsigned int)(responseChar[i]) << " ";
								}
								responseOut.close();
#endif
								backpack_->sendMessage(responseStruct->m_EMsg, responseBuffer.start(), responseSize);
								free( response );
							}
							break;
						case k_EMsgGCUpdateItemSchema:
							{
								// Not handling yet.
								break;
							}
						case GCCraftResponse_t::k_iMessage:
							{
								GCCraftResponse_t *craftMsg = (GCCraftResponse_t*)buffer;
								if (craftMsg->blueprint == 0xffff) {
									createAlert("Crafting failed. No such blueprint exists!");
								}
								else {
									createAlert("Congratulations! Item crafting succeeded!");
								}

								break;
							}

						default:
							backpack_->handleMessage( id, buffer, size - sizeof( GCProtobufHeader_t ) - headerSize );
							break;
						}

						// Free up memory.
						if (buffer != nullptr) {
							free(buffer);
							google::protobuf::ShutdownProtobufLibrary();
						}
					}
				}
			}
		}

		backpack_->releaseCallback();
	}
}

Button* ItemManager::createButton(const string& caption, Texture *texture, float x, float y, EAlignment align)
{
	// Create and add.
	Button* newButton = new Button(caption, texture, x, y, align);

	// Add and return.
	buttonList_.push_back(newButton);
	return newButton;
}

Dialog* ItemManager::createDialog(const string& message)
{
	Dialog* newDialog = new Dialog(message);
	add(newDialog);

	// Set position.
	float x = (float)(getWidth() / 2) - (float)(newDialog->getWidth() / 2);
	float y = (float)(getHeight() / 2) - (float)(newDialog->getHeight() / 2);
	newDialog->setPosition(x, y);
	newDialog->setParent(this);

	// Show popup.
	showPopup(newDialog);

	// Add and return.
	popupList_.push_back(newDialog);
	return newDialog;
}

Alert* ItemManager::createAlert(const string& message)
{
	Alert* newAlert = new Alert(message);
	add(newAlert);

	const string* str = &message;
	const char* msg = message.c_str();

	// Set position.
	float alertX = (float)(getWidth() / 2) - (newAlert->getWidth() / 2);
	float alertY = (float)(getHeight() / 2) - (newAlert->getHeight() / 2);
	newAlert->setPosition(alertX, alertY);
	newAlert->setParent(this);

	// Show popup.
	showPopup(newAlert);

	// Add and return.
	popupList_.push_back(newAlert);
	return newAlert;
}

void ItemManager::handlePopup(Popup *popup)
{
	switch (popup->getState()) {
		case POPUP_STATE_INACTIVE:
			hidePopup(popup);
			break;
		case POPUP_STATE_KILLED:
			removePopup(popup);
			break;
	}
}

void ItemManager::showPopup(Popup* popup)
{
	popup->setState(POPUP_STATE_ACTIVE);
	popupStack_.push_back(popup);
}

void ItemManager::hidePopup(Popup *popup)
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

void ItemManager::removePopup(Popup* popup)
{
	// Hide and remove.
	hidePopup(popup);
	remove(popup);
	delete popup;
}