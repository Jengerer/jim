#ifndef ITEMINFORMATION_H
#define ITEMINFORMATION_H

#include "Texture.h"
#include "steam/SteamAPI.h"
#include <string>

using namespace std;

//Class ownership enum.
enum EItemClass {
	CLASS_NONE,
	CLASS_SCOUT,
	CLASS_SOLDIER,
	CLASS_PYRO,
	CLASS_DEMOMAN,
	CLASS_HEAVY,
	CLASS_ENGINEER,
	CLASS_MEDIC,
	CLASS_SNIPER,
	CLASS_SPY,
	CLASS_ANY
};

//Item slot enum.
enum EItemSlot {
	SLOT_NONE,
	SLOT_PRIMARY,
	SLOT_SECONDARY,
	SLOT_MELEE,
	SLOT_PDA1,
	SLOT_PDA2,
	SLOT_BUILDING,
	SLOT_HAT,
	SLOT_MISCELLANEOUS
};

//Stores item information.
class ItemInformation
{
public:
	ItemInformation(int newType, 
		string newName, 
		EItemClass newClass, 
		EItemSlot newSlot,
		Texture* newTexture);
	~ItemInformation();
	
	/* Item attribute getters. */
	uint32		getType() const;
	string		getName() const;
	EItemClass	getClass() const;
	EItemSlot	getSlot() const;

	/* Texture getter. */
	Texture*	getTexture();

private:
	/* Item attributes. */
	uint32		m_itemType;
	string		m_itemName;
	EItemClass	m_itemClass;
	EItemSlot	m_itemSlot;

	/* Graphical elements. */
	Texture*	m_lpTexture;
};

#endif //ITEMINFORMATION_H