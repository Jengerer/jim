#pragma once

#include "Steam.h"
#include "Curl.h"
#include "Item.h"
#include "Slot.h"
#include <vector>

class Inventory: public Steam
{
public:
	Inventory(int newWidth, int newHeight, 
		int pageCount);
	~Inventory();

	/* Initializing and ending. */
	void					loadInterfaces();
	void					closeInterfaces();

	// Slot handling.
	void					createSlots();
	void					clearSlots();
	void					moveItem(Slot* oldSlot, Slot* newSlot);
	bool					slotValid(uint8 whichIndex);
	Slot*					getSlot(uint8 whichIndex);
	const vector<Slot*>*	getSlots();

	// State methods.
	void					setLoaded();
	bool					isLoaded() const;

	// Item handling.
	void					addItem(Item *newItem);
	void					deleteItem(Item *whichItem);
	void					selectItem(Item *whichItem);
	void					clearItems();

	// Item selection methods.
	void					deleteSelected();
	void					craftSelected();

	// Steam callback handling.
	void					handleCallbacks();

	// Size getters.
	int						getWidth() const;
	int						getHeight() const;
	int						getPageSize() const;
	int						getPages() const;
	int						getCapacity() const;

private:
	// Vector for all items.
	vector<Item*>			m_vItems;

	// Slot vectors.
	vector<Slot*>			m_vInventory;
	vector<Slot*>			m_vSelected;
	vector<Slot*>			m_vExcluded;

	// Status variables.
	bool					m_isLoaded;

	// Dimensions.
	int						m_iWidth, m_iHeight;
	int						m_iPages;
};