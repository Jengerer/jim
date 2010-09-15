#include "ItemInformation.h"

ItemInformation::ItemInformation(int newType, 
								 string newName,
								 EItemClass newClass,
								 EItemSlot newSlot,
								 Texture* newTexture)
{
	m_itemType = newType;
	m_itemName = newName;
	m_itemClass = newClass;
	m_itemSlot = newSlot;

	m_lpTexture = newTexture;
}

ItemInformation::~ItemInformation()
{
	//ItemInformation has been destroyed.
}

uint32 ItemInformation::getType() const
{
	return m_itemType;
}

string ItemInformation::getName() const
{
	return m_itemName;
}

EItemClass ItemInformation::getClass() const
{
	return m_itemClass;
}

EItemSlot ItemInformation::getSlot() const
{
	return m_itemSlot;
}

Texture* ItemInformation::getTexture()
{
	return m_lpTexture;
}