#pragma once

#include "steam/SteamAPI.h"
#include "steam/SteamTypes.h"
#include "steam/UserItemsCommon.h"
#include "Hashtable.h"
#include "Drawable.h"
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

class Item: public Drawable
{
public:
	bool operator==(const Item &other) const; //TODO: { return (this->GetUniqueID() == rOther.GetUniqueID()); }
	bool operator!=(const Item &other) const; //TODO: { return (this->GetUniqueID() != rOther.GetUniqueID()); }

	/* Constructor. */
	Item(uint64 uniqueId,
		uint32 defIndex,
		uint32 level,
		EItemQuality quality,
		uint32 count,
		uint32 flags );
	~Item();

	/* Initializing item. */
	void				loadInformation();

	// Item attribute getters.
	uint64				getUniqueId() const;
	uint32				getType() const;
	uint32				getLevel() const;
	EItemQuality		getQuality() const;
	uint32				getFlags() const;
	uint32				getCount() const;

	// Secondary attribute getters.
	string				getName();
	bool				isHat() const;
	bool				isEquipped() const;
	string				getSlot() const;
	EItemGroup			getGroup() const;
	uint8				getPosition() const;

	// Modify item attributes.
	void				setEquip( int classIndex, bool equip );
	void				setPosition( uint8 position );

	// Drawing and interaction.
	Texture*			getTexture();
	bool				isTouching() const;

	// Item information table.
	static Hashtable*	informationTable;

	// Drawing functions.
	void				drawObject( DirectX* directX );

	// Input handling.
	void				onMouseEvent( MouseListener* mouseListener, EMouseEvent mEvent );

	// Size getters.
	int					getWidth();
	int					getHeight();

private:
	// Item information.
	uint64				uniqueId_;
	uint32				defIndex_;
	uint32				level_;
	EItemQuality		quality_;
	uint32				count_;
	uint32				flags_;

	// Secondary information.
	uint8				position_;
	EItemGroup			group_;

	// Item definition information.
	const Hashtable*	information_;

	// Pointers from the information.
	string*				name_;
	Texture*			texture_;
};