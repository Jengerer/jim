#pragma once

#include "Item.h"

class Slot
{
public:
	Slot();
	Slot(Item* whichItem);

	void setItem(Item* newItem);
	Item* getItem();
	bool isEmpty() const;

private:
	Item*	m_pItem;
};