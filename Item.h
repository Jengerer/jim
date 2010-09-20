#ifndef ITEM_H
#define ITEM_H

#include "steam/EItemQuality.h"
#include "ItemInformation.h"
#include "Hashtable.h"
#include <vector>
#include <string>
#include <sstream>

using namespace std;

//Item type for manager handling.
enum EItemGroup {
	GROUP_INVENTORY,
	GROUP_EXCLUDED
};

//Item selection type for manager handling.
enum EItemSelection {
	SELECT_NONE,
	SELECT_NORMAL,
	SELECT_DRAG
};

class Item
{
public:
	bool operator==(const Item &rOther) const; //TODO: { return (this->GetUniqueID() == rOther.GetUniqueID()); }
	bool operator!=(const Item &rOther) const; //TODO: { return (this->GetUniqueID() != rOther.GetUniqueID()); }

	/* Constructor. */
	Item(uint64 newUniqueID,
		uint32 newType,
		uint32 newLevel,
		EItemQuality newQuality,
		uint32 newQuantity,
		uint32 newAttributes,
		uint32 newFlags);
	~Item();

	/* Initializing item. */
	void				getInformation();

	/* Item attribute getters. */
	uint64				getUniqueID() const;
	uint32				getType() const;
	uint32				getLevel() const;
	EItemQuality		getQuality() const;
	uint32				getFlags() const;
	uint32				getQuantity() const;
	uint32				getAttributes() const;

	/* Secondary attribute getters. */
	string					getName();
	bool					isHat() const;
	bool					isEquipped() const;
	EItemClass				getClass() const;
	string					getSlot() const;
	EItemGroup				getGroup() const;
	EItemSelection			getSelection() const;
	uint8					getPosition() const;

	/* Item attribute setters. */
	void				setGroup(EItemGroup newGroup);
	void				setSelect(EItemSelection newSelection);

	/* Modifying item attributes. */
	void				setEquip(int whichClass, bool doEquip);
	void				setPosition(uint8 newPosition);

	/* Drawing and interaction. */
	Texture*			getTexture();
	bool				isVisible() const;
	bool				isTouching() const;

	/* Public information table. */
	static Hashtable*	m_hInformation;

private:
	uint64				m_itemID;
	uint32				m_itemType;
	uint32				m_itemLevel;
	EItemQuality		m_itemQuality;
	uint32				m_itemQuantity;
	uint32				m_itemAttributes;
	uint32				m_itemFlags;

	uint8				m_itemPosition;

	EItemSelection		m_itemSelection;
	EItemGroup			m_itemGroup;

	Hashtable*		m_pInformation;

	string*				m_itemName;
	Texture*			m_lpTexture;
};

#endif //ITEM_H