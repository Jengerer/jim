#pragma once

#include <vector>
#include <string>
#include <sstream>

#include <boost/lexical_cast.hpp>

#include "steam/SteamAPI.h"
#include "steam/SteamTypes.h"
#include "steam/UserItemsCommon.h"

#include "Hashtable.h"
#include "Image.h"
#include "Drawable.h"

using namespace std;

enum EClassEquip {
	CLASS_NONE = 0x00000000,
	CLASS_SCOUT = 0x00010000,
	CLASS_SOLDIER = 0x00040000,
	CLASS_PYRO = 0x00400000,
	CLASS_DEMOMAN = 0x00080000,
	CLASS_HEAVY = 0x00200000,
	CLASS_ENGINEER = 0x01000000,
	CLASS_MEDIC = 0x00100000,
	CLASS_SNIPER = 0x00020000,
	CLASS_SPY = 0x00800000,
	CLASS_ALL = 0x01FB4000
};

class Item: public Image
{
public:
	bool operator==(const Item &other) const; // TODO: { return (this->GetUniqueID() == rOther.GetUniqueID()); }
	bool operator!=(const Item &other) const; // TODO: { return (this->GetUniqueID() != rOther.GetUniqueID()); }

	// Constructor.
	Item(
		uint64 uniqueId,
		uint16 defIndex,
		uint8 level,
		EItemQuality quality,
		uint32 count,
		uint32 flags );
	virtual ~Item();

	/* Initializing item. */
	void				loadInformation();

	// Drawable functions.
	void				draw( DirectX* directX );
	virtual int			getWidth() const;
	virtual int			getHeight() const;

	// Item attribute getters.
	uint64				getUniqueId() const;
	uint16				getDefIndex() const;
	uint8				getLevel() const;
	EItemQuality		getQuality() const;
	uint32				getFlags() const;
	uint32				getCount() const;

	// Secondary attribute getters.
	string				getName();
	bool				isHat() const;
	bool				isEquipped( EClassEquip equipClass = CLASS_ALL ) const;
	string				getSlot() const;
	uint16				getIndex() const;
	Hashtable*			getClasses() const;

	// Modify item attributes.
	void				setEquip( EClassEquip equipClass, bool equip );
	void				move( uint16 position );

	// Drawing and interaction.
	Texture*			getTexture();
	bool				isTouching() const;

	// Item information table.
	static Hashtable*	informationTable;

private:
	// Item information.
	uint64				uniqueId_;
	uint16				defIndex_;
	uint8				level_;
	EItemQuality		quality_;
	uint32				count_;
	uint32				flags_;

	// Secondary information.
	uint16				position_;

	// Item definition information.
	const Hashtable*	information_;

	// Pointers from the information.
	string*				name_;
	Texture*			texture_;
};