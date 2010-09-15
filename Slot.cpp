#include "Slot.h"

Slot::Slot(Item* whichItem)
{
	setItem(whichItem);
}

Slot::Slot()
{
	setItem(NULL);
}

void Slot::setItem(Item* whichItem)
{
	m_pItem = whichItem;
}

Item* Slot::getItem()
{
	return m_pItem;
}

bool Slot::isEmpty() const
{
	return (m_pItem == NULL);
}