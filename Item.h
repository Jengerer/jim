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

class Item: public Image
{
public:
	bool operator==(const Item &other) const; // TODO: { return (this->GetUniqueID() == rOther.GetUniqueID()); }
	bool operator!=(const Item &other) const; // TODO: { return (this->GetUniqueID() != rOther.GetUniqueID()); }

	// Constructor.
	Item(
		uint64 uniqueId,
		uint32 defIndex,
		uint32 level,
		EItemQuality quality,
		uint32 count,
		uint32 flags );
	virtual ~Item();

	/* Initializing item. */
	void				loadInformation();

	// Drawable functions.
	void				draw( DirectX* directX );
	int					getWidth() const;
	int					getHeight() const;

	// Item attribute getters.
	uint64				getUniqueId() const;
	uint32				getDefIndex() const;
	uint32				getLevel() const;
	EItemQuality		getQuality() const;
	uint32				getFlags() const;
	uint32				getCount() const;

	// Secondary attribute getters.
	string				getName();
	bool				isHat() const;
	bool				isEquipped() const;
	string				getSlot() const;
	uint16				getIndex() const;

	// Modify item attributes.
	void				setEquip( int classIndex, bool equip );
	void				move( uint16 position );

	// Drawing and interaction.
	Texture*			getTexture();
	bool				isTouching() const;

	// Item information table.
	static Hashtable*	informationTable;

private:
	// Item information.
	uint64				uniqueId_;
	uint32				defIndex_;
	uint32				level_;
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