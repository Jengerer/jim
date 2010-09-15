#pragma once

#include "KeyValueParser.h"
#include "Steam.h"
#include "Curl.h"
#include "Item.h"
#include "Slot.h"
#include <vector>

class Inventory: public Steam
{
public:
	Inventory(Curl* pCurl,
		int newWidth, int newHeight, 
		int pageCount);
	~Inventory();

	/* Initializing and ending. */
	bool loadInterfaces();
	void closeInterfaces();

	// Inventory and item handling.
	void clearItems();

	// Steam callback handling.
	void handleCallbacks();

	// Handling inventory.
	void clearSlots();
	vector<Slot*>* getInventory();

	// Handling items and slots.
	void createSlots();
	bool slotValid(uint8 whichIndex);
	void addItem(Item *newItem);
	void deleteItem(Item *whichItem);
	Slot* getSlot(uint8 whichIndex);

	/* Size getters. */
	int getWidth() const;
	int getHeight() const;
	int getPageSize() const;
	int getPages() const;
	uint8 getCapacity() const;

	/* Handling individual items. */
	void moveItem(Slot* oldSlot, Slot* newSlot);
	void selectItem(Item *whichItem);

	/* Multiple item actions. */
	void deleteSelected();
	void craftSelected();

private:
	/* Vectors for storing items. */
	vector<Item*> m_vItems;

	/* Vector for holding slots. */
	vector<Slot*> m_vInventory;
	vector<Slot*> m_vSelected;
	vector<Slot*> m_vExcluded;

	/* Inventory variables. */
	bool m_isLocked, m_isLoaded;

	/* Application interfaces. */
	Curl*		m_pCurl;

	/* Dimensions. */
	int m_iWidth, m_iHeight;
	int m_iPages;
};