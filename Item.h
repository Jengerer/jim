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
	Item( uint64 uniqueId,
		uint16 typeIndex,
		uint8 level,
		EItemQuality quality,
		uint32 count,
		uint32 flags );
	virtual ~Item( void );

	// Loading item definition by index.
	void			GetItemInformation( void );

	// Drawable functions.
	virtual void	OnDraw( DirectX* directX );

	// Item attribute getters.
	uint64			GetUniqueId( void ) const;
	uint16			GetTypeIndex( void ) const;
	uint8			GetLevel( void ) const;
	EItemQuality	GetQuality( void ) const;
	uint32			GetFlags( void ) const;
	uint32			GetCount( void ) const;

	// Secondary attributes.
	const string&	GetName( void ) const;
	uint16			GetIndex( void ) const ;
	void			SetIndex( uint16 position );

	// Equipment handling.
	bool			IsEquipped( EClassEquip equipClass = CLASS_ALL ) const;
	const string&	GetEquipSlot( void ) const;
	Hashtable*		GetEquipClasses( void ) const;
	void			SetEquip( EClassEquip equipClass, bool equip );

	// Drawing and interaction.
	Texture*		GetTexture( void );

private:

	void SetUniqueId( uint64 uniqueId );
	void SetTypeIndex( uint16 typeIndex );
	void SetLevel( uint8 level );
	void SetQuality( EItemQuality quality );
	void SetFlags( uint32 flags );
	void SetCount( uint32 count );

public:

	// Item information table.
	static Hashtable*	informationTable;

private:
	// Item information.
	uint64				uniqueId_;
	uint16				typeIndex_;
	uint8				level_;
	EItemQuality		quality_;
	uint32				count_;
	uint32				flags_;

	// Item definition information.
	const Hashtable*	information_;

	// Pointers from the information.
	Texture*			texture_;
};