#ifndef ITEM_H
#define ITEM_H

#include "steam/SteamAPI.h"
#include "steam/SteamTypes.h"
#include "steam/UserItemsCommon.h"
#include "item_shared.hpp"
#include "item_definition.hpp"
#include "equip_status.hpp"

#include <containers/vector.hpp>
#include <containers/map.hpp>
#include <string/string.hpp>
#include <string/constant_string.hpp>
#include <jui/gfx/texture.hpp>

// Strange attribute constants.
const unsigned int DEFAULT_STRANGE_TYPE = 0;
const unsigned int STRANGE_TYPE_COUNT = 5;

// Item flags.
const unsigned int UNTRADABLE_FLAG = 0x1;

class Item
{
public:
	// Constructor.
	Item( uint64 unique_id,
		uint16 typeIndex,
		uint8 level,
		EItemQuality quality,
		uint32 count,
		uint32 inventory_flags,
		uint32 flags,
		uint32 origin );
	virtual ~Item( void );

	// Item definition resolving.
	const ItemDefinition* get_definition( void ) const;
	void set_definition( const ItemDefinition* definition );
	bool set_custom_name( const JUTIL::String* custom_name );
	bool generate_name( void );
	void update_attributes( void );

	// Item attribute getters.
	uint64			get_unique_id( void ) const;
	uint16			get_type_index( void ) const;
	uint8			get_level( void ) const;
	EItemQuality	get_quality( void ) const;
	uint32			get_inventory_flags( void ) const;
	uint32			get_flags( void ) const;
	void			set_inventory_flags( uint32 inventory_flags );
	uint32			get_count( void ) const;
	uint32			get_origin( void ) const;
	bool			is_renamed( void ) const;
	void			set_renamed( bool renamed );
	EItemToolType	get_tool_type( void ) const;

	// Secondary attributes.
	const JUTIL::String* get_name( void ) const;
	const JUI::Colour* get_quality_colour( void ) const;

	// Position stored in Steam.
	void set_position( uint16 position );
	uint16 get_position( void ) const;

    // Item flag management.
	bool has_valid_inventory_flags( void ) const;

	// Equipment handling.
	bool is_tradable( void ) const;
	uint32 get_inventory_classes( void ) const;
	bool class_uses( uint32 class_flags ) const;
	uint8 get_inventory_class_count( void ) const;

	// Attribute based getters
	bool get_crate_number( uint32* out ) const;
	bool get_craft_number( uint32* out ) const;
	bool get_kill_eater_value( uint32 index, uint32* out ) const;
	bool get_kill_eater_type( uint32 index, uint32* out ) const;

	// Drawing and interaction.
	const JUI::Texture* get_texture( void ) const;

	// Attribute management.
	bool add_attribute( Attribute* attribute );

    // Attribute iteration.
    size_t get_attribute_count( void ) const;
	size_t get_local_attribute_count( void ) const;
	Attribute* get_local_attribute( size_t index ) const;
    const Attribute* get_attribute( size_t index ) const;
    const Attribute* find_attribute( size_t index ) const;
	const Attribute* find_attribute( const JUTIL::String* name ) const;

	// Selection management.
	void set_selected( bool selected );
	bool is_selected( void ) const;

	// Equipped data management
	bool add_equipped_data( EEquipClass equip_class, EEquipSlot slot );
	bool remove_equipped_data( EEquipClass equip_class );
    size_t get_equipped_count( void ) const;
    const EquippedStatus* get_equipped_status( size_t index ) const;
	const EquippedStatus* find_equipped_status( EEquipClass equip_class ) const;
	bool is_equipped( void ) const;

private:

	void set_unique_id( uint64 unique_id );
	void set_type_index( uint16 type_index );
	void set_level( uint8 level );
	void set_quality( EItemQuality quality );
	void set_count( uint32 count );
	void set_origin( uint32 origin );
	void set_flags( uint32 flags );

private:

	// Item information.
	uint64			unique_id_;
	uint16			type_index_;
	uint8			level_;
	EItemQuality	quality_;
	uint32			count_;
	uint32			inventory_flags_;
	uint32			flags_;
	uint32			origin_;
	bool			renamed_;

	// Secondary information.
	JUTIL::DynamicString item_name_;
	bool is_selected_;

	// Item definition information.
	const ItemDefinition* definition_;
	JUTIL::Vector<Attribute*> attributes_;
	JUTIL::Vector<EquippedStatus> equipped_data_;

};

#endif // ITEM_H