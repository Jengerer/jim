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

#include <jui/texture.h>

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

	// Finish creating item.
	void			get_item_information( void );
	void			update_attributes( void );

	// Item attribute getters.
	uint64			get_unique_id( void ) const;
	uint16			get_type_index( void ) const;
	uint8			get_level( void ) const;
	EItemQuality	get_quality( void ) const;
	uint32			get_flags( void ) const;
	void			set_flags( uint32 flags );
	uint32			get_count( void ) const;
	uint32			get_origin( void ) const;

	// Secondary attributes.
	std::string		get_name( void ) const;
	const Colour&	get_quality_colour( void ) const;
	const char*		get_quality_name( void ) const;

	// Position stored in Steam.
	void			set_position( uint16 position );
	uint16			get_position( void ) const;

	// Index in slot array.
	uint32			get_index( void ) const;
	void			set_index( uint32 position );

	bool			has_valid_flags( void ) const;
	void			set_custom_name( const std::string& name );
	const std::string& get_custom_name( void ) const;
	bool			has_custom_name( void ) const;

	// Equipment handling.
	bool			is_tradable( void ) const;
	bool			is_equipped( uint32 equipClass = CLASS_ALL ) const;
	EItemSlot		get_equip_slot( void ) const;
	uint32			get_equip_classes( void ) const;
	bool			class_uses( uint32 classFlags ) const;
	uint8			get_equip_class_count( void ) const;
	void			set_equip( uint32 equipClass, bool equip );

	// Drawing and interaction.
	const Texture*	get_texture( void );

	// Attribute handling.
	void add_attribute( Attribute* attribute );
	size_t get_attribute_count() const;
	const Attribute* get_attribute_at( size_t index ) const;
	const Attribute* get_attribute_by_index( size_t index ) const;
	const Attribute* get_attribute_by_name( const std::string& name ) const;

private:

	void set_unique_id( uint64 uniqueId );
	void set_type_index( uint16 typeIndex );
	void set_level( uint8 level );
	void set_quality( EItemQuality quality );
	void set_count( uint32 count );
	void set_origin( uint32 origin );

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