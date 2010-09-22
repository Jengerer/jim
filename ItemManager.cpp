#include "ItemManager.h"

using namespace std;

// Window properties.
#define APPLICATION_TITLE		"Jengerer's Item Manager"
#define APPLICATION_WIDTH		795
#define APPLICATION_HEIGHT		500

// Application attributes.
#define APPLICATION_FRAMERATE	30.0f
#define APPLICATION_FRAMESPEED	1000.0f/APPLICATION_FRAMERATE
#define APPLICATION_VERSION		1000

// Inventory attributes.
#define PAGE_WIDTH	10
#define PAGE_HEIGHT	5
#define PAGE_COUNT	2

// Drawing constants.
#define PADDING			25.0f
#define SLOT_SPACING	5.0f

// Main application variable.
Main* pApplication = NULL;

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_MOUSEMOVE:
		if (pApplication != NULL) pApplication->pollMouse();
		break;
	case WM_LBUTTONDOWN:
		if (pApplication != NULL) pApplication->onMouseDown();
		break;
	case WM_LBUTTONUP:
		if (pApplication != NULL) pApplication->onMouseUp();
		break;
	}

	return DefWindowProc(hWnd, iMsg, wParam, lParam);
}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	try
	{
		pApplication = new ItemManager(hInstance);
		pApplication->loadInterfaces();
	} catch (Exception mainException)
	{
		MessageBox(NULL, mainException.getMessage().c_str(), "Initialization failed!", MB_OK);
		return EXIT_FAILURE;
	}

	bool isDone = false;

	MSG iMsg;
	while (!isDone)
	{
		if (PeekMessage(&iMsg, NULL, 0, 0, PM_REMOVE))
		{
			if (iMsg.message == WM_QUIT)
				isDone = true;

			TranslateMessage(&iMsg);
			DispatchMessage(&iMsg);
		}

		pApplication->onFrame();
	}

	if (pApplication != NULL)
	{
		delete pApplication;
		pApplication = NULL;
	}

	return EXIT_SUCCESS;
}

ItemManager::ItemManager(HINSTANCE hInstance): DirectX(APPLICATION_TITLE, hInstance, APPLICATION_WIDTH, APPLICATION_HEIGHT)
{
}

ItemManager::~ItemManager()
{
	// ItemManager has been destroyed.
	closeInterfaces();
}

void ItemManager::loadInterfaces()
{
	// Create inventory.
	m_pInventory = new Inventory(this, PAGE_WIDTH, PAGE_HEIGHT, PAGE_COUNT);

	// Load the slot texture.
	// TODO: have a table of texture names and URLs; should all load at once.
	Slot::m_slotTexture = getTexture("manager/item_slot");

	// Define and load items.
	loadDefinitions();
	loadItems();
}

void ItemManager::closeInterfaces()
{
	/* Delete Item Information. */
	if (Item::m_hInformation != NULL)
	{
		delete Item::m_hInformation;
		Item::m_hInformation = NULL;
	}

	// Delete inventory.
	if (m_pInventory != NULL)
	{
		delete m_pInventory;
		m_pInventory = NULL;
	}
}

#include <cmath>

Item* lastItem = NULL;
void ItemManager::onRedraw()
{
	const vector<Slot*>* m_vpInventory = m_pInventory->getInventory();
	vector<Slot*>::const_iterator pSlot;

	int itemIndex = 0;
	// TODO: Don't draw slots off-screen.
	for (pSlot = m_vpInventory->begin(); pSlot != m_vpInventory->end(); pSlot++)
	{
		Slot* thisSlot = *pSlot;

		int xIndex = (itemIndex % PAGE_WIDTH);
		int yIndex = (itemIndex / PAGE_WIDTH);

		float fX = PADDING, fY = PADDING;

		// Overflow second page to off-screen.
		if (yIndex >= PAGE_HEIGHT)
		{
			yIndex %= PAGE_HEIGHT;
			fX += getWidth();
		}

		// Set position.
		thisSlot->m_fX = fX + (thisSlot->getWidth() + SLOT_SPACING)*xIndex;
		thisSlot->m_fY = fY + (thisSlot->getHeight() + SLOT_SPACING)*yIndex;

		// Draw it.
		thisSlot->drawObject(this);

		// Increment.
		itemIndex++;
	}

	lastItem = NULL;
}

void ItemManager::onFrame()
{
	redrawScreen();
}

void ItemManager::onMouseDown()
{
	// Check for hitting.
	/*
	for (int I=0; I<m_pInventory->getCapacity(); I++)
	{
		Slot* whichSlot = m_pInventory->getSlot(I);
		if (!whichSlot->isEmpty() && m_pMouse->isTouching(whichSlot))
		{
			Item* thisItem = whichSlot->getItem();

			// Toggle selection.
			m_pInventory->selectItem(thisItem);

			break;
		}
	}*/
}

void ItemManager::onMouseUp()
{
	/* Mouse released. */
}

void ItemManager::loadDefinitions()
{
	// Load the item definitions.
	string itemDefinitions = readFile("http://www.jengerer.com/itemManager/itemDefinitions.json");

	// Begin parsing.
	Json::Reader jsonReader;
	Json::Value jsonRoot;
	if (!jsonReader.parse(itemDefinitions, jsonRoot, false))
	{
		throw Exception("Failed to parse item definitions.");
	}

	Item::m_hInformation = new Hashtable();
	for (Json::ValueIterator jsonIterator = jsonRoot.begin(); jsonIterator != jsonRoot.end(); jsonIterator++)
	{
		Json::Value thisItem = *jsonIterator;
		
		bool hasKeys = thisItem.isMember("itemIndex") &&
			thisItem.isMember("itemName") &&
			thisItem.isMember("itemSlot") &&
			thisItem.isMember("imageURL") &&
			thisItem.isMember("imageInventory");

		if (!hasKeys)
		{
			throw Exception("Failed to parse item definitions. One or more missing members from item entry.");
		}

		// Get strings.
		string stringIndex = thisItem.get("itemIndex", jsonRoot).asString();
		string itemName = thisItem.get("itemName", jsonRoot).asString();
		string imageInventory = thisItem.get("imageInventory", jsonRoot).asString();
		string itemSlot = thisItem.get("itemSlot", jsonRoot).asString();

		// Make sure there's a file.
		string imageURL;
		if (imageInventory.length() == 0)
		{
			imageInventory = "backpack/unknown_item";
			imageURL = "http://www.jengerer.com/itemManager/imgFiles/backpack/unknown_item.png";
		} else
		{
			imageURL = thisItem.get("imageURL", jsonRoot).asString();
		}

		// Add strings to new table.
		Hashtable* thisTable = new Hashtable();
		thisTable->put("itemName", new string(itemName));
		thisTable->put("imageURL", new string(imageURL));
		thisTable->put("imageInventory", new string(imageInventory));
		thisTable->put("itemSlot", new string(itemSlot));

		try
		{
			Texture* itemTexture = getTexture(imageInventory);
			thisTable->put("itemTexture", itemTexture);
		} catch (Exception &textureException)
		{
			if (thisTable != NULL)
			{
				delete thisTable;
				thisTable = NULL;
			}

			throw textureException;
		}

		Item::m_hInformation->put(stringIndex, thisTable);
	}
}

void ItemManager::loadItems()
{
	/* First clear all vectors. */
	m_pInventory->clearItems();

	/* Get user's Steam community URL. */
	uint64 userID = m_pInventory->getSteamID();

	stringstream urlStream;
	urlStream << "http://api.steampowered.com/ITFItems_440/GetPlayerItems/v0001/?key=0270F315C25E569307FEBDB67A497A2E&SteamID=" << userID << "&format=json";
	string communityURL = urlStream.str();

	/* Items loaded in read-only. */
	setLocked(true);
	setLoaded(true);

	// Attempt to read the file.
	string jsonInventory;
	try
	{
		jsonInventory = readFile(communityURL);
	} catch (Exception curlException)
	{
		throw Exception("Failed to read inventory from profile.");
	}
	
	// Begin parsing the string.
	Json::Reader jsonReader;
	Json::Value jsonRoot;

	// Attempt to parse it.
	if (!jsonReader.parse(jsonInventory, jsonRoot, false))
	{
		throw Exception("Failed to parse inventory JSON file.");
	}

	// Get the result.
	if (!jsonRoot.isMember("result"))
	{
		throw Exception("Failed to parse player's items from Web API: no 'result' key received.");
	}

	Json::Value resultValue = jsonRoot.get("result", jsonRoot);
	if (!resultValue.isMember("status"))
	{
		throw Exception("Failed to parse player's items from Web API: no 'status' key received.");
	}

	// Get the JSON value for the status.
	int intStatus = resultValue.get("status", jsonRoot).asInt();
	if (intStatus == 15)
	{
		throw Exception("Failed to parse player's items from Web API: no permissions.");
	} else if (intStatus == 8)
	{
		throw Exception("Failed to parse player's items from Web API: invalid SteamID argument.");
	}

	if (!resultValue.isMember("items"))
	{
		throw Exception("Failed to parse player's items from Web API: no 'items' key received.");
	}

	// Get the items object.
	Json::Value itemsObject = resultValue.get("items", jsonRoot);
	
	// TODO: Check that 'item' exists in an empty inventory.
	if (!itemsObject.isMember("item"))
	{
		throw Exception("Failed to parse player's items from Web API: no 'item' key received.");
	}

	// Get the array of items.
	Json::Value itemsArray = itemsObject.get("item", jsonRoot);
	
	// Keep track of attributes.
	SOMsgCacheSubscribed_Item_t* addedItem = new SOMsgCacheSubscribed_Item_t();

	for (Json::ValueIterator jsonIterator = itemsArray.begin(); jsonIterator != itemsArray.end(); jsonIterator++)
	{
		Json::Value thisItem = *jsonIterator;

		bool hasKeys = thisItem.isMember("id") && 
			thisItem.isMember("defindex") && 
			thisItem.isMember("level") && 
			thisItem.isMember("quality") && 
			thisItem.isMember("inventory") &&
			thisItem.isMember("quantity");

		if (!hasKeys)
		{
			throw Exception("Failed to parse player's items from Web API: unexpected format for items received.");
		}

		addedItem->itemid = thisItem.get("id", jsonRoot).asUInt64();
		addedItem->itemdefindex = thisItem.get("defindex", jsonRoot).asUInt();
		addedItem->itemlevel = thisItem.get("level", jsonRoot).asUInt();
		addedItem->itemquality = thisItem.get("quality", jsonRoot).asUInt();
		addedItem->position = thisItem.get("inventory", jsonRoot).asUInt();

		// All attributes retrieved, make item.
		Item* newItem = new Item(
			addedItem->itemid,
			addedItem->itemdefindex,
			addedItem->itemlevel,
			(EItemQuality)addedItem->itemquality,
			addedItem->itemcount,
			addedItem->position);

		// Classify item by slot validity.
		uint8 newPosition = newItem->getPosition();
		Slot* newSlot = m_pInventory->getSlot(newPosition);
		if ((newSlot != NULL) && (newSlot->m_pItem == NULL))
		{
			newItem->setGroup(GROUP_INVENTORY);
		} else
		{
			newItem->setGroup(GROUP_EXCLUDED);
		}

		// Add the item.
		m_pInventory->addItem(newItem);
	}

	if (addedItem != NULL)
	{
		delete addedItem;
		addedItem = NULL;
	}

	//TODO: Update excluded scrolling.
}

void ItemManager::handleCallbacks()
{
	//Get waiting callback.
	CallbackMsg_t tCallback;
	if (m_pInventory->getCallback(&tCallback))
	{
		switch (tCallback.m_iCallback)
		{
		case GCMessageAvailable_t::k_iCallback:
			{
				GCMessageAvailable_t *gcMsg = (GCMessageAvailable_t *)tCallback.m_pubParam;
				
				uint32 iSize = 0;
				if (m_pInventory->hasMessage(&iSize))
				{
					unsigned int messageID;
					unsigned int sizeReal = 0;

					/* Allocate memory. */
					void* messageBuffer;
					messageBuffer = malloc(iSize);

					/* Retrieve the message. */
					m_pInventory->getMessage(&messageID, messageBuffer, iSize, &sizeReal);

					switch (messageID)
					{
					case SOMsgCacheSubscribed_t::k_iMessage:
						//Item information has been received.
						{
							if (!isLoaded())
							{
								
								setLoaded(true);
								setLocked(false);

								//Start loading items.
								SerializedBuffer thisBuffer(messageBuffer);
								SOMsgCacheSubscribed_t *iList = thisBuffer.getValue<SOMsgCacheSubscribed_t>();
								for (int I = 0; I < iList->itemcount; I++)
								{
									SOMsgCacheSubscribed_Item_t *pItem = thisBuffer.getValue<SOMsgCacheSubscribed_Item_t>();

									// Skip past the name.
									thisBuffer.pushBuffer(pItem->namelength);

									// Get attribute count, and skip past.
									uint16* attribCount = thisBuffer.getValue<uint16>();
									thisBuffer.pushBuffer<SOMsgCacheSubscribed_Item_Attrib_t>(*attribCount);

									//Create a new item from the information.
									Item *newItem = new Item(
										pItem->itemid,
										pItem->itemdefindex,
										pItem->itemlevel,
										(EItemQuality)pItem->itemquality,
										pItem->itemcount,
										pItem->position);

									// Add the item.
									m_pInventory->addItem(newItem);
								}

								//TODO: Update scrolling of excluded items.
							}
							
							break;
						}

					case GCCraftResponse_t::k_iMessage:
						{
							GCCraftResponse_t *pResponse = (GCCraftResponse_t*)messageBuffer;

							if (pResponse->blueprint == 0xFFFF)
							{
								//Crafting failed; work in a message box.
								//TODO: Display the dialog for crafting failed.
							}

							break;
						}
					case SOMsgCreate_t::k_iMessage:
						{
							SOMsgCreate_t *createdMsg = (SOMsgCreate_t*)messageBuffer;
							SOMsgCacheSubscribed_Item_t* craftedItem = &createdMsg->item;

							//Make sure it's a valid item.
							if (createdMsg->unknown == 5)
								break;

							Item* newItem = new Item(
								craftedItem->itemid,
								craftedItem->itemdefindex,
								craftedItem->itemlevel,
								(EItemQuality)craftedItem->itemquality,
								craftedItem->itemcount,
								craftedItem->position);

							/* Add this item to the excluded. */
							newItem->setGroup(GROUP_EXCLUDED);
							m_pInventory->addItem(newItem);
							//TODO: Update excluded scrolling.

							break;
						}
					case SOMsgDeleted_t::k_iMessage:
						{
							SOMsgDeleted_t *pDeleted = (SOMsgDeleted_t*)messageBuffer;

							/* Just find the item and remove it. */
							Steam* inventorySteam = m_pInventory;
							inventorySteam->deleteItem(pDeleted->itemid);

							break;
						}
					}

					/* Free up memory. */
					if (messageBuffer != NULL)
						free(messageBuffer);
				}
				
			}
		}
	}

	m_pInventory->releaseCallback();
}

void ItemManager::setLocked(bool isLocked)
{
	m_bLocked = isLocked;
}

void ItemManager::setLoaded(bool isLoaded)
{
	m_bLoaded = isLoaded;
}

bool ItemManager::isLocked() const
{
	return m_bLocked;
}

bool ItemManager::isLoaded() const
{
	return m_bLoaded;
}