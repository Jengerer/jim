#pragma once

#include <vector>
#include <string>
#include <map>

#include <boost/lexical_cast.hpp>

#include "steam/SteamAPI.h"
#include "steam/SteamTypes.h"
#include "steam/UserItemsCommon.h"

#include "attribute_information.h"
#include "exception.h"
#include "item_information.h"
#include "item_shared.h"
#include "string_hasher.h"
#include "texture.h"

typedef std::map<int, ItemInformation*> InformationMap;
typedef std::map<uint32, AttributeInformation*>	AttributeMap;

class Item
{
public:
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

	// Item attribute getters.
	uint64			GetUniqueId( void ) const;
	uint16			GetTypeIndex( void ) const;
	uint8			GetLevel( void ) const;
	EItemQuality	GetQuality( void ) const;
	uint32			GetFlags( void ) const;
	void			SetFlags( uint32 flags );
	uint32			GetCount( void ) const;

	// Secondary attributes.
	string			GetName( void ) const;
	const Colour&	GetQualityColour( void ) const;
	const char*		GetQualityName( void ) const;

	// Position stored in Steam.
	void			SetPosition( uint16 position );
	uint16			GetPosition( void ) const;

	// Index in slot array.
	uint32			GetIndex( void ) const;
	void			SetIndex( uint32 position );

	bool			HasValidFlags( void ) const;
	void			SetCustomName( const string& name );
	const string&	GetCustomName( void ) const;
	bool			HasCustomName( void ) const;

	// Equipment handling.
	bool			IsEquipped( uint32 equipClass = CLASS_ALL ) const;
	EItemSlot		GetEquipSlot( void ) const;
	uint32			GetEquipClasses( void ) const;
	bool			ClassUses( uint32 classFlags ) const;
	uint8			GetEquipClassCount( void ) const;
	void			SetEquip( uint32 equipClass, bool equip );

	// Drawing and interaction.
	Texture*		GetTexture( void );

private:

	void SetUniqueId( uint64 uniqueId );
	void SetTypeIndex( uint16 typeIndex );
	void SetLevel( uint8 level );
	void SetQuality( EItemQuality quality );
	void SetCount( uint32 count );

public:

	// Using int32 so we can have -1 index for fallback.
	// TODO: Keep it as uint16, but store a static fallback definition.
	static InformationMap definitions;
	static AttributeMap attributes;

private:

	// Item information.
	uint64					uniqueId_;
	uint16					typeIndex_;
	uint8					level_;
	EItemQuality			quality_;
	uint32					count_;
	uint32					flags_;

	// Secondary information.
	string customName_;
	uint32 index_;

	// Item definition information.
	const ItemInformation	*information_;

};