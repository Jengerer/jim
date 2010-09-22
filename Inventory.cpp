#include "Inventory.h"

Inventory::Inventory(Curl* pCurl,
	int newWidth, int newHeight, 
	int pageCount)
{
	// Set cURL pointer.
	m_pCurl = pCurl;

	// Set dimensions.
	m_iWidth = newWidth;
	m_iHeight = newHeight;
	m_iPages = pageCount;

	// Inventory has been created.
	loadInterfaces();
}

Inventory::~Inventory()
{
	//Inventory has been deleted.
	closeInterfaces();
}

void Inventory::loadInterfaces()
{
	/* Create slots for items. */
	createSlots();
}

void Inventory::closeInterfaces()
{
	/* Remove all slots and items. */
	clearSlots();
	clearItems();
}

void Inventory::createSlots()
{
	/* Create inventory slots. */
	for (int I=0; I<getCapacity(); I++)
	{
		Slot* thisSlot = new Slot();
		m_vInventory.push_back(thisSlot);
	}
}

void Inventory::clearItems()
{
	/* Iterator for all vectors. */
	vector<Item*>::iterator pItem;

	/* Delete inventory. */
	for (pItem = m_vItems.begin(); pItem != m_vItems.end(); pItem++)
		delete *pItem;
	
	/* Now just empty the vector. */
	m_vItems.clear();
}

void Inventory::clearSlots()
{
	/* Iterator for slots. */
	vector<Slot*>::iterator pSlot;

	/* Delete inventory slots. */
	for (pSlot = m_vInventory.begin(); pSlot != m_vInventory.end(); pSlot++)
		delete *pSlot;

	/* Delete excluded slots. */
	for (pSlot = m_vExcluded.begin(); pSlot != m_vExcluded.end(); pSlot++)
		delete *pSlot;

	/* Delete selected slots. */
	for (pSlot = m_vSelected.begin(); pSlot != m_vSelected.end(); pSlot++)
		delete *pSlot;

	/* Now clear vectors. */
	m_vInventory.clear();
	m_vExcluded.clear();
	m_vSelected.clear();
}

void Inventory::addItem(Item* newItem)
{
	/* Add it to the item list. */
	m_vItems.push_back(newItem);

	/* Add this item to correct slot. */
	uint8 itemPosition = newItem->getPosition() - 1;
	if ((itemPosition >= 0) && (itemPosition < getCapacity()))
	{
		Slot* movedSlot = m_vInventory[itemPosition];
		/* Don't overlap items. */
		if (movedSlot->m_pItem == NULL)
		{
			newItem->setGroup(GROUP_INVENTORY);
			movedSlot->m_pItem = newItem;
		} else
		{
			newItem->setGroup(GROUP_EXCLUDED);

			/* Make new slot for excluded. */
			movedSlot = new Slot(newItem);
			m_vExcluded.push_back(movedSlot);
		}
	}
}

void Inventory::deleteItem(Item* whichItem)
{
	/* Iterator for searching. */
	vector<Item*>::iterator pSearcher;
	uint64 itemID = whichItem->getUniqueID();

	/* Cycle through selected for item. */
	for (pSearcher = m_vItems.begin(); pSearcher != m_vItems.end(); pSearcher++)
	{
		Item* thisItem = *pSearcher;
		if (thisItem->getUniqueID() == itemID)
		{
			/* Send delete message. */
			Steam::deleteItem(itemID);

			/* Delete the slot with this item. */
			vector<Slot*>::iterator pSlot;
			for (pSlot = m_vSelected.begin(); pSlot != m_vSelected.end(); pSlot++)
			{
				Slot* thisSlot = *pSlot;
				if (thisSlot->m_pItem == thisItem)
				{
					delete thisSlot;
					m_vSelected.erase(pSlot);
					break;
				}
			}

			/* Delete the item object. */
			delete thisItem;
			m_vItems.erase(pSearcher);

			break;
		}
	}
}

Slot* Inventory::getSlot(uint8 whichIndex)
{
	// Search for that slot, return NULL if invalid.
	return (slotValid(whichIndex) ? m_vInventory[whichIndex] : NULL);
}

int Inventory::getWidth() const
{
	return m_iWidth;
}

int Inventory::getHeight() const
{
	return m_iHeight;
}

int Inventory::getPageSize() const
{
	return getWidth()*getHeight();
}

int Inventory::getPages() const
{
	return m_iPages;
}

uint8 Inventory::getCapacity() const
{
	return getPageSize()*getPages();
}

void Inventory::moveItem(Slot *oldSlot, Slot* newSlot)
{
	/* Get the item we're moving. */
	Item* itemMoved = oldSlot->m_pItem;

	if (newSlot->m_pItem == NULL)
	{
		newSlot->m_pItem = itemMoved;
		oldSlot->m_pItem = NULL;

		//m_pSteam->updateItem(itemMoved);
	} else if (itemMoved->getGroup() == GROUP_INVENTORY)
	{
		Item* replacedItem = newSlot->m_pItem;
		newSlot->m_pItem = itemMoved;
		oldSlot->m_pItem = replacedItem;

		//m_pSteam->updateItem(itemMoved);
		//m_pSteam->updateItem(replacedItem);
	}
}

bool Inventory::slotValid(uint8 whichIndex)
{
	return ((whichIndex >= 0) && (whichIndex < getCapacity()));
}

void Inventory::selectItem(Item *whichItem)
{
	whichItem->setSelect(whichItem->getSelection() == SELECT_NONE ? SELECT_DRAG : SELECT_NONE);
}

vector<Slot*>* Inventory::getInventory()
{
	return &m_vInventory;
}