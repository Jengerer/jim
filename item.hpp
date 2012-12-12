#ifndef ITEM_H
#define ITEM_H

#include "steam/SteamAPI.h"
#include "steam/SteamTypes.h"
#include "steam/UserItemsCommon.h"
#include "item_shared.hpp"
#include "item_definition.hpp"

#include <containers/vector.hpp>
#include <containers/map.hpp>
#include <string/string.hpp>
#include <string/constant_string.hpp>
#include <jui/gfx/texture.hpp>

class Item
{
public:
	// Constructor.
	Item( uint64 unique_id,
		uint16 typeIndex,
		uint8 level,
		EItemQuality quality,
		uint32 count,
		uint32 flags,
		uint32 origin );
	virtual ~Item( void );

	// Item definition resolving.
	void set_definition( const ItemDefinition* definition );
	bool set_custom_name( const JUTIL::String* custom_name );
	bool generate_name( void );
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
	const JUTIL::String* get_origin_name( void ) const;

	// Position stored in Steam.
	void set_position( uint16 position );
	uint16 get_position( void ) const;

	// Index in slot array.
	uint32 get_index( void ) const;
	void set_index( uint32 position );

    // Item flag management.
	bool has_valid_flags( void ) const;

	// Equipment handling.
	bool is_tradable( void ) const;
	bool is_equipped( uint32 equip_class = CLASS_ALL ) const;
	EItemSlot get_equip_slot( void ) const;
	uint32 get_equip_classes( void ) const;
	bool class_uses( uint32 class_flags ) const;
	uint8 get_equip_class_count( void ) const;
	void set_equip( uint32 equip_class, bool equip );

	uint32 get_crate_number( void ) const;
	uint32 get_paint_value( uint32 index ) const;

	// Drawing and interaction.
	const JUI::Texture* get_texture( void );

	// Attribute management.
	bool add_attribute( Attribute* attribute );

    // Attribute iteration.
    size_t get_attribute_count( void ) const;
	Attribute* get_attribute( size_t index );
    const Attribute* get_attribute( size_t index ) const;
    const Attribute* find_attribute( size_t index ) const;
	const Attribute* find_attribute( const JUTIL::String* name ) const;

private:

	void set_unique_id( uint64 unique_id );
	void set_type_index( uint16 typeIndex );
	void set_level( uint8 level );
	void set_quality( EItemQuality quality );
	void set_count( uint32 count );
	void set_origin( uint32 origin );

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
	JUTIL::DynamicString item_name_;

	// Item definition information.
	uint32 index_;
	const ItemDefinition* definition_;
	JUTIL::Vector<Attribute*> attributes_;

};

#endif // ITEM_H