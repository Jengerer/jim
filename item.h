#pragma once

#include <vector>
#include <string>
#include <map>

#include <boost/lexical_cast.hpp>

#include "steam/SteamAPI.h"
#include "steam/SteamTypes.h"
#include "steam/UserItemsCommon.h"

#include "attribute_information.h"
#include <stdexcept>
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
		uint32 flags,
		uint32 origin );
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
	uint32			GetOrigin( void ) const;

	// Secondary attributes.
	std::string		GetName( void ) const;
	const Colour&	GetQualityColour( void ) const;
	const char*		GetQualityName( void ) const;

	// Position stored in Steam.
	void			SetPosition( uint16 position );
	uint16			GetPosition( void ) const;

	// Index in slot array.
	uint32			GetIndex( void ) const;
	void			SetIndex( uint32 position );

	bool			HasValidFlags( void ) const;
	void			SetCustomName( const std::string& name );
	const std::string& GetCustomName( void ) const;
	bool			HasCustomName( void ) const;

	// Equipment handling.
	bool			IsEquipped( uint32 equipClass = CLASS_ALL ) const;
	EItemSlot		GetEquipSlot( void ) const;
	uint32			GetEquipClasses( void ) const;
	bool			ClassUses( uint32 classFlags ) const;
	uint8			GetEquipClassCount( void ) const;
	void			SetEquip( uint32 equipClass, bool equip );

	// Drawing and interaction.
	const Texture*	GetTexture( void );

	// Attribute handling.
	void add_attribute( Attribute* attribute );
	size_t get_attribute_count() const;
	const Attribute* get_attribute_at( size_t index ) const;
	const Attribute* get_attribute_by_index( size_t index ) const;
	const Attribute* get_attribute_by_name( const std::string& name ) const;

private:

	void SetUniqueId( uint64 uniqueId );
	void SetTypeIndex( uint16 typeIndex );
	void SetLevel( uint8 level );
	void SetQuality( EItemQuality quality );
	void SetCount( uint32 count );
	void SetOrigin( uint32 origin );

public:

	// Using int32 so we can have -1 index for fallback.
	// TODO: Keep it as uint16, but store a static fallback definition.
	static ItemInformation* fallback;
	static InformationMap definitions;
	static AttributeMap attributes;

private:

	// Item information.
	uint64			uniqueId_;
	uint16			typeIndex_;
	uint8			level_;
	EItemQuality	quality_;
	uint32			count_;
	uint32			flags_;
	uint32			origin_;

	// Secondary information.
	std::string customName_;
	uint32 index_;

	// Item definition information.
	const ItemInformation*	information_;
	std::vector<Attribute*>	attributes_;

};