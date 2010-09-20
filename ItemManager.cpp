#include "ItemManager.h"

using namespace std;

#define PAGE_WIDTH	10
#define PAGE_HEIGHT	5
#define PAGE_COUNT	2

#define SLOT_SPACING 5

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
	try
	{
		loadInterfaces();
	} catch (Exception managerException)
	{
		closeInterfaces();
		throw managerException;
	}
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

void ItemManager::onRedraw()
{
	const vector<Slot*>* m_vpInventory = m_pInventory->getInventory();
	vector<Slot*>::const_iterator pSlot;
	float thisX = 50.0f, thisY = 50.0f;
	for (pSlot = m_vpInventory->begin(); pSlot != m_vpInventory->end(); pSlot++)
	{
		Slot* thisSlot = *pSlot;
		Item* thisItem = thisSlot->getItem();

		Texture* thisTexture = thisItem->getTexture();
		drawTexture(thisTexture, thisX, thisY);

		thisX += 10.0f;
	}
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
	Json::Value *jsonRoot = new Json::Value();
	if (!jsonReader.parse(itemDefinitions, *jsonRoot, false))
		throw Exception("Failed to parse item definitions.");
	delete jsonRoot;

	Item::m_hInformation = new Hashtable();
}

void ItemManager::loadItems()
{
	/* First clear all vectors. */
	m_pInventory->clearItems();

	/* Get user's Steam community URL. */
	uint64 userID = m_pInventory->getSteamID();

	stringstream urlStream;
	urlStream << "steamcommunity.com/profiles/" << userID << "/tfitems?json=1";
	string communityURL = urlStream.str();

	/* Items loaded in read-only. */
	setLocked(true);
	setLoaded(true);

	/* Attempt to read the JSON file. */
	string jsonInventory;
	try
	{
		jsonInventory = readFile(communityURL);
	} catch (Exception curlException)
	{
		throw Exception("Failed to read inventory from profile.");
	}
	
	/* Begin parsing the string. */
	stringstream jsonFile(jsonInventory);

	bool doWrite = false;
	string keyWord = "";

	vector<string> wordLibrary;
	int bracketScope = 0;

	while (jsonFile.good())
	{
		char currentChar = jsonFile.get();

		//Check if access denied.
		if (currentChar == '[')
			throw Exception ("Cound not load items in read-only mode. Profile is private.");

		/* Skip attributes. */
		if ((currentChar == '{') && (bracketScope == 2))
		{
			while (jsonFile.good() && (currentChar != '}'))
				currentChar = jsonFile.get();

			doWrite = false;
			continue;
		}

		/* Write what we have now. */
		if (doWrite)
		{
			if ((bracketScope == 1) && (currentChar == '"'))
			{
				/* End of item ID. */
				doWrite = false;
				wordLibrary.push_back(keyWord);

				keyWord = "";
			} else if ((bracketScope == 2) && ((currentChar == ',') || (currentChar == '}')))
			{
				/* End of number. */
				doWrite = false;
				wordLibrary.push_back(keyWord);

				keyWord = "";
			} else
			{
				/* Otherwise, we're just writing. */
				keyWord += currentChar;
			}
		} else
		{
			/* Begin writing if we've reached an opening quote. */
			if (((bracketScope == 1) && (currentChar == '"')) ||
				((bracketScope == 2) && (currentChar == ':')))
				doWrite = true;
		}

		/* Alter scope accordingly. */
		if (currentChar == '{')
			bracketScope++;
		else if (currentChar == '}')
			bracketScope--;
	}

	/* Keep track of an item's attributes. */
	SOMsgCacheSubscribed_Item_t* addedItem = new SOMsgCacheSubscribed_Item_t();

	/* Iterate through the words found. */
	vector<string>::const_iterator keyWords;
	int whichWord = 0;

	for (keyWords = wordLibrary.begin(); keyWords != wordLibrary.end(); keyWords++)
	{
		switch (whichWord++ % 6)
		{
		case 0:
			/* This is the item ID. */
			addedItem->itemid = atol(keyWords->c_str());
			break;

		case 1:
			/* This is the item type. */
			addedItem->itemdefindex = atoi(keyWords->c_str());
			break;

		case 2:
			/* This is the item level. */
			addedItem->itemlevel = atoi(keyWords->c_str());
			break;

		case 3:
			/* This is the item quality. */
			addedItem->itemquality = (EItemQuality)atoi(keyWords->c_str());
			break;

		case 4:
			/* This is the item flags. */
			addedItem->position = atol(keyWords->c_str());
			break;

		case 5:
			/* This is the item quantity. */
			addedItem->itemcount = atoi(keyWords->c_str());

			/* All attributes accounted for; add the item. */
			Item* newItem = new Item(
				addedItem->itemid,
				addedItem->itemdefindex,
				addedItem->itemlevel,
				(EItemQuality)addedItem->itemquality,
				addedItem->itemcount,
				addedItem->attribcount,
				addedItem->position);

			/* Classify item based on slot validity. */
			uint8 newPosition = newItem->getPosition();
			Slot* newSlot = m_pInventory->getSlot(newPosition);
			if ((newSlot != NULL) && (newSlot->isEmpty()))
				newItem->setGroup(GROUP_INVENTORY);
			else
				newItem->setGroup(GROUP_EXCLUDED);

			/* Add the item, and move to next. */
			m_pInventory->addItem(newItem);
			break;
		}
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
		case GameCoordinatorMessageAvailable_t::k_iCallback:
			{
				GameCoordinatorMessageAvailable_t *gcMsg = (GameCoordinatorMessageAvailable_t *)tCallback.m_pubParam;
				
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
								SOMsgCacheSubscribed_t *iList = (SOMsgCacheSubscribed_t *)messageBuffer;

								void *itemIterator = ((unsigned char *)(iList) + sizeof(SOMsgCacheSubscribed_t));
								for (int I = 0; I < iList->itemcount; I++)
								{
									SOMsgCacheSubscribed_Item_t *pItem = (SOMsgCacheSubscribed_Item_t *)itemIterator;

									//Skip ahead to the next item.
									for (int X = 0; X < pItem->attribcount; X++)
										itemIterator = ((unsigned char *)itemIterator + sizeof(SOMsgCacheSubscribed_Item_Attrib_t));

									//Create a new item from the information.
									Item *newItem = new Item(
										pItem->itemid,
										pItem->itemdefindex,
										pItem->itemlevel,
										(EItemQuality)pItem->itemquality,
										pItem->itemcount,
										pItem->attribcount,
										pItem->position);

									delete newItem;

									/* Add the item, and move to next. */
									//m_pInventory->addItem(newItem);
									itemIterator = ((unsigned char *)itemIterator + sizeof(SOMsgCacheSubscribed_Item_t));
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
							SOMsgCreate_t *craftedItem = (SOMsgCreate_t*)messageBuffer;

							//Make sure it's a valid item.
							if (craftedItem->unknown == 5)
								break;

							Item* newItem = new Item(
								craftedItem->itemid,
								craftedItem->itemtype,
								craftedItem->itemlevel,
								(EItemQuality)craftedItem->itemquality,
								craftedItem->itemcount,
								craftedItem->attribcount,
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