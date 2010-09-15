#include "Item.h"

const uint32 EQUIP_FLAGS[] = {
	0x00000000, //None
	0x00010000, //Scout
	0x00040000,
	0x00400000,
	0x00080000,
	0x00200000,
	0x01000000,
	0x00100000,
	0x00020000,
	0x00800000 //Spy
};

/* Create definitions hashtable. */
stringHashMap* Item::m_hInformation = NULL;

Item::Item(uint64 newUniqueID,
		   uint32 newType,
		   uint32 newLevel,
		   EItemQuality newQuality,
		   uint32 newQuantity,
		   uint32 newAttributes,
		   uint32 newFlags)
{
	/* Set basic attributes. */
	m_itemID = newUniqueID;
	m_itemType = newType;
	m_itemLevel = newLevel;
	m_itemQuality = newQuality;
	m_itemQuantity = newQuantity;
	m_itemAttributes = newAttributes;
	m_itemFlags = newFlags;

	/* Set null for things to be gotten. */
	m_lpTexture = NULL;
	m_itemName = NULL;
	m_pInformation = NULL;

	/* Load item's definition. */
	getInformation();

	/* Set selection and position. */
	setSelect(SELECT_NONE);
	m_itemPosition = m_itemFlags & 0xFF;
}

Item::~Item()
{
	//Item has been destroyed.
}

void Item::getInformation()
{
	/* Convert item type to string. */
	stringstream typeStream;
	typeStream << getType();
	string thisType = typeStream.str();

	stringHashMap::iterator hashIterator = m_hInformation->find(thisType);
	if (hashIterator != m_hInformation->end())
		m_pInformation = (stringHashMap*)hashIterator->second;
	else
		m_pInformation = (stringHashMap*)(*m_hInformation)["-1"];
}

uint64 Item::getUniqueID() const
{
	return m_itemID;
}

uint32 Item::getType() const
{
	return m_itemType;
}

uint32 Item::getLevel() const
{
	return m_itemLevel;
}

EItemQuality Item::getQuality() const
{
	return m_itemQuality;
}

uint32 Item::getFlags() const
{
	return m_itemFlags;
}

uint32 Item::getQuantity() const
{
	return m_itemQuantity;
}

uint32 Item::getAttributes() const
{
	return m_itemAttributes;
}

uint8 Item::getPosition() const
{
	return m_itemPosition;
}

string Item::getName()
{
	if (m_itemName == NULL)
		m_itemName = (string*)(*m_pInformation)["itemName"];

	return *m_itemName;
}

bool Item::isHat() const
{
	return (getSlot() == "hat");
}

string Item::getSlot() const
{
	string* thisSlot = (string*)(*m_pInformation)["itemSlot"];
	return *thisSlot;
}

EItemGroup Item::getGroup() const
{
	return m_itemGroup;
}

EItemSelection Item::getSelection() const
{
	return m_itemSelection;
}

void Item::setGroup(EItemGroup newGroup)
{
	m_itemGroup = newGroup;
}

void Item::setSelect(EItemSelection newSelection)
{
	m_itemSelection = newSelection;
}

void Item::setPosition(uint8 newPosition)
{
	//Set to new position.
	m_itemPosition = newPosition;

	//If item has improper flags, fix them.
	uint32 newFlags = m_itemFlags & 0xFFFFFF00;
	if ((m_itemFlags & 0xF0000000) != 0x80000000)
		newFlags = 0x80000000;

	//Put new position.
	newFlags += newPosition;
	m_itemFlags = newFlags;
}

bool Item::isEquipped() const
{
	int equipFlags = m_itemFlags & 0x0FFFFF00;
	int validFlags = m_itemFlags & 0xF0000000;
	return ((validFlags == 0x80000000) && (equipFlags != 0x00000000));
}

void Item::setEquip(int whichClass, bool doEquip)
{
	if (m_itemFlags & EQUIP_FLAGS[whichClass])
	{
		//This item is equipped to that class; remove them.
		if (!doEquip)
			m_itemFlags -= EQUIP_FLAGS[whichClass];
	} else
	{
		//This item is not equipped.
		if (doEquip)
			m_itemFlags += EQUIP_FLAGS[whichClass];
	}
}

Texture* Item::getTexture()
{
	if (m_lpTexture == NULL)
		m_lpTexture = (Texture*)(*m_pInformation)["itemTexture"];
	
	return m_lpTexture;
}