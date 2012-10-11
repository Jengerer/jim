#ifndef ITEM_H
#define ITEM_H

#include <containers/vector.hpp>
#include <containers/map.hpp>
#include <string/string.hpp>
#include <string/constant_string.hpp>

#include "steam/SteamAPI.h"
#include "steam/SteamTypes.h"
#include "steam/UserItemsCommon.h"

#include "attribute_information.hpp"
#include "item_information.hpp"
#include "item_shared.hpp"
#include "string_hasher.hpp"

#include <jui/gfx/texture.hpp>

typedef JUTIL::Map<int, ItemInformation*> InformationMap;
typedef JUTIL::Map<uint16, AttributeInformation*> AttributeMap;

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

    // Initializing item name/attributes.
    void clean( void );

	// Finish creating item.
	bool update_item_information( JUTIL::String* custom_name );
	void update_attributes( void );

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
	const JUTIL::String* get_name( void ) const;
	const JUI::Colour* get_quality_colour( void ) const;
	const JUTIL::String* get_quality_name( void ) const;

	// Position stored in Steam.
	void			set_position( uint16 position );
	uint16			get_position( void ) const;

	// Index in slot array.
	uint32			get_index( void ) const;
	void			set_index( uint32 position );

    // Item flag management.
	bool has_valid_flags( void ) const;

	// Equipment handling.
	bool			is_tradable( void ) const;
	bool			is_equipped( uint32 equipClass = CLASS_ALL ) const;
	EItemSlot		get_equip_slot( void ) const;
	uint32			get_equip_classes( void ) const;
	bool			class_uses( uint32 classFlags ) const;
	uint8			get_equip_class_count( void ) const;
	void			set_equip( uint32 equipClass, bool equip );

	// Drawing and interaction.
	const JUI::Texture* get_texture( void );

	// Attribute handling.
	bool add_attribute( Attribute* attribute );
	size_t get_attribute_count( void ) const;
	const Attribute* get_attribute_at( size_t index ) const;
	const Attribute* get_attribute_by_index( size_t index ) const;
	const Attribute* get_attribute_by_name( const JUTIL::String* name ) const;

private:

	void set_unique_id( uint64 uniqueId );
	void set_type_index( uint16 typeIndex );
	void set_level( uint8 level );
	void set_quality( EItemQuality quality );
	void set_count( uint32 count );
	void set_origin( uint32 origin );

public:

	static ItemInformation* fallback;
	static InformationMap definitions;
	static AttributeMap attributes;

private:

	// Item information.
	uint64			unique_id_;
	uint16			type_index_;
	uint8			level_;
	EItemQuality	quality_;
	uint32			count_;
	uint32			flags_;
	uint32			origin_;

	// Secondary information.
	JUTIL::String* item_name_;
	uint32 index_;

	// Item definition information.
	const ItemInformation* information_;
	JUTIL::Vector<Attribute*> attributes_;

};

#endif // ITEM_H