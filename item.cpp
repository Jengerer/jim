#include "item.hpp"
#include <jui/application/error_stack.hpp>

const JUTIL::ConstantString CANNOT_TRADE_NAME = "cannot trade";
const JUTIL::ConstantString ALWAYS_TRADE_NAME = "always tradable";
const JUTIL::ConstantString CRAFT_NUMBER_ATTRIBUTE_NAME = "unique craft index";
const JUTIL::ConstantString UNLIMITED_COUNT_ATTRIBUTE_NAME = "unlimited quantity";
const JUTIL::ConstantString CRATE_ATTRIBUTE_NAME = "set supply crate series";

// Names of attributes that give us the value of the kill eater.
const JUTIL::ConstantString STRANGE_ATTRIBUTE_NAMES[STRANGE_TYPE_COUNT] =
{
	JUTIL::ConstantString( "kill eater" ),
	JUTIL::ConstantString( "kill eater 2" ),
	JUTIL::ConstantString( "kill eater user 1" ),
	JUTIL::ConstantString( "kill eater user 2" ),
	JUTIL::ConstantString( "kill eater user 3" )
};

// Names of attributes that give us the type of kill eater.
const JUTIL::ConstantString STRANGE_TYPE_NAMES[STRANGE_TYPE_COUNT] =
{
	JUTIL::ConstantString( "kill eater score type" ),
	JUTIL::ConstantString( "kill eater score type 2" ),
	JUTIL::ConstantString( "kill eater user score type 1" ),
	JUTIL::ConstantString( "kill eater user score type 2" ),
	JUTIL::ConstantString( "kill eater user score type 3" )
};

/*
 * Item constructor from attributes.
 */
Item::Item(
	uint64 unique_id,
	uint16 type_index,
	uint8 level,
	EItemQuality quality,
	uint32 count,
	uint32 inventory_flags,
	uint32 flags,
	uint32 origin )
{
	// Set basic attributes.
	set_unique_id( unique_id );
	set_type_index( type_index );
	set_level( level );
	set_quality( quality );
	set_count( count );
	set_inventory_flags( inventory_flags );
	set_flags( flags );
	set_origin( origin );
	set_renamed( false );
	set_selected( false );
}

/*
 * Item destructor; remove attributes.
 */
Item::~Item( void )
{
	// Remove all attributes.
    size_t i;
    size_t length = attributes_.get_length();
    for (i = 0; i < length; ++i) {
        Attribute* attribute = attributes_.at( i );
        JUTIL::BaseAllocator::destroy( attribute );
    }
    attributes_.clear();

	// Remove all equipped data.
    length = equipped_data_.get_length();
    for (i = 0; i < length; ++i) {
		EquippedStatus* datum = &equipped_data_.at( i );
        JUTIL::BaseAllocator::destroy( datum );
    }
    equipped_data_.clear();
}

/*
 * Get item definition structure from schema.
 */
const ItemDefinition* Item::get_definition( void ) const
{
	return definition_;
}

/*
 * Set item definition structure from schema.
 */
void Item::set_definition( const ItemDefinition* definition )
{
	definition_ = definition;
}

// Set item custom name.
bool Item::set_custom_name( const JUTIL::String* custom_name )
{
	// Copy custom name with quotes.
    if (!item_name_.write( "\"%s\"", custom_name->get_string() )) {
        return false;
    }
	set_renamed( true );
	return true;
}

/*
 * Resolve item name from definitions.
 */
bool Item::generate_name( void )
{
	// Error stack for logging.
	JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

    // Get item type name.
    const JUTIL::String* type_name = definition_->get_name();
    if (!item_name_.write( "%s", type_name->get_string() )) {
		stack->log( "Failed to write item type name when generating name string." );
        return false;
    }

    return true;
}

/*
 * Update item attributes based on attribute list.
 */
void Item::update_attributes( void )
{
	// Check if quality is elevated.
    const JUTIL::ConstantString ELEVATED_QUALITY_NAME = "elevated quality";
	const Attribute* quality_attrib = find_attribute( &ELEVATED_QUALITY_NAME );
	if (quality_attrib != nullptr) {
		// First get quality number.
		const float* value = reinterpret_cast<const float*>(quality_attrib->get_value());
		uint32 quality_num = static_cast<uint32>(*value);
		set_quality( static_cast<EItemQuality>(quality_num) );
	}
}

/*
 * Get item's 64-bit unique ID.
 */
uint64 Item::get_unique_id( void ) const
{
	return unique_id_;
}

/*
 * Get item's type index.
 */
uint16 Item::get_type_index( void ) const
{
	return type_index_;
}

/*
 * Get item level.
 */
uint8 Item::get_level( void ) const
{
	return level_;
}

/*
 * Get item quality enumeration.
 */
EItemQuality Item::get_quality( void ) const
{
	return quality_;
}

/*
 * Get inventory flags for item.
 */
uint32 Item::get_inventory_flags( void ) const
{
	return inventory_flags_;
}

/*
 * Get extra flags for item.
 */
uint32 Item::get_flags( void ) const
{
	return flags_;
}

/*
 * Get item count.
 */
uint32 Item::get_count( void ) const
{
	// Check if we have infinite use attribute.
	const Attribute* attribute = find_attribute( &UNLIMITED_COUNT_ATTRIBUTE_NAME );
	if (attribute == nullptr) {
		return count_;
	}
	return FL_ITEM_UNLIMITED_COUNT;
}

/*
 * Get item origin.
 */
uint32 Item::get_origin( void ) const
{
	return origin_;
}

/*
 * Get item tool type.
 */
EItemToolType Item::get_tool_type( void ) const
{
	return definition_->get_tool_type();
}

/*
 * Is item renamed.
 */
bool Item::is_renamed( void ) const
{
	return renamed_;
}

/*
 * Set renamed.
 */
void Item::set_renamed( bool renamed )
{
	renamed_ = renamed;
}

/*
 * Get string handle to item name.
 */
const JUTIL::String* Item::get_name( void ) const
{
    return &item_name_;
}

/*
 * Get colour for item quality.
 */
const JUI::Colour* Item::get_quality_colour( void ) const
{
	switch (get_quality()) {
	case k_EItemQuality_Vintage:
		return &QUALITY_VINTAGE_COLOUR;
		break;

	case k_EItemQuality_Genuine:
		return &QUALITY_GENUINE_COLOUR;
		break;

	case k_EItemQuality_Unusual:
		return &QUALITY_UNUSUAL_COLOUR;
		break;

	case k_EItemQuality_Haunted:
		return &QUALITY_HAUNTED_COLOUR;
		break;

	case k_EItemQuality_Strange:
		return &QUALITY_STRANGE_COLOUR;
		break;

	case k_EItemQuality_Community:
	case k_EItemQuality_SelfMade:
		return &QUALITY_COMMUNITY_COLOUR;
		break;

	case k_EItemQuality_Valve:
		return &QUALITY_VALVE_COLOUR;
		break;

	case k_EItemQuality_Collectors:
		return &QUALITY_COLLECTORS_COLOUR;
		break;

	default:
		return &QUALITY_COMMON_COLOUR;
		break;
	}
}

/*
 * Get item position index.
 * Position indices are considered to start at 0 and end at one less than index count.
 */
uint16 Item::get_position( void ) const
{
	return (get_inventory_flags() & FL_ITEM_POSITION) - 1;
}

/*
 * Set item new position index.
 * Position indices are considered to start at 0 and end at one less than index count.
 */
void Item::set_position( uint16 position )
{
	uint32 tempFlags = has_valid_inventory_flags() ? get_inventory_flags() : FL_ITEM_VALID;
	tempFlags &= FL_ITEM_NONPOSITION; // Remove current position.
	tempFlags |= (position + 1) & FL_ITEM_POSITION; // Set new.
	set_inventory_flags( tempFlags );
}

/*
 * Return whether this item containts valid flags.
 */
bool Item::has_valid_inventory_flags( void ) const
{
	return (get_inventory_flags() & 0xF0000000) == FL_ITEM_VALID;
}

/* Checks whether an item is allowed to be traded. */
bool Item::is_tradable( void ) const
{
	return ((get_origin() != ORIGIN_ACHIEVEMENT) &&
		((get_flags() & UNTRADABLE_FLAG) == 0) &&
		(find_attribute( &CANNOT_TRADE_NAME ) == nullptr)) ||
		(find_attribute( &ALWAYS_TRADE_NAME ) != nullptr);
}

/* Checks if the equip flags match this item. */
bool Item::class_uses( uint32 class_flags ) const
{
	return (get_inventory_classes() & class_flags) != 0;
}

/* Gets all item equip flags. */
uint32 Item::get_inventory_classes( void ) const
{
	return definition_->get_class_flags();
}

/*
 * Get number of classes that this item can be equipped on.
 */
uint8 Item::get_inventory_class_count( void ) const
{
	return definition_->get_class_count();
}

/*
 * Gets the crate number. Returns false if no crate number, fills into out otherwise.
 */
bool Item::get_crate_number( uint32* out ) const
{
	const Attribute* attribute = find_attribute( &CRATE_ATTRIBUTE_NAME );
	if (attribute == nullptr) {
		return false;
	}
	const float* as_float = reinterpret_cast<const float*>(attribute->get_value());
	*out = static_cast<uint32>(*as_float);
	return true;
}

/*
 * Gets the craft number.  Returns false if none available, fills into out if exists.
 */
bool Item::get_craft_number( uint32* out ) const
{
	const Attribute* attribute = find_attribute( &CRAFT_NUMBER_ATTRIBUTE_NAME );
	if (attribute == nullptr) {
		return false;
	}
	const uint32* as_uint32 = reinterpret_cast<const uint32*>(attribute->get_value());
	*out = *as_uint32;
	return true;
}

/*
 * Gets the value of a given kill eater index and fills it into out.
 * Returns true if one was found at that index.
 */
bool Item::get_kill_eater_value( uint32 index, uint32* out ) const
{
	const Attribute* attribute = find_attribute( &STRANGE_ATTRIBUTE_NAMES[index] );
	if (attribute == nullptr) {
		return false;
	}
	const uint32* as_uint32 = reinterpret_cast<const uint32*>(attribute->get_value());
	*out = *as_uint32;
	return true;
}

/*
 * Gets the index type of kill eater for a given index.
 * Returns true if a kill eater type was found at that index and fills out
 * the type index into out.
 */
bool Item::get_kill_eater_type( uint32 index, uint32* out ) const
{
	// Get the attribute by name.
	const Attribute* attribute = find_attribute( &STRANGE_TYPE_NAMES[index] );
	if (attribute == nullptr) {
		return false;
	}
	const float* as_float = reinterpret_cast<const float*>(attribute->get_value());
	uint32 as_uint32 = static_cast<uint32>(*as_float);
	*out = as_uint32;
	return true;
}

/*
 * Get the texture for this item, if any.
 */
const JUI::Texture* Item::get_texture( void ) const
{
    return definition_->get_texture();
}

/*
 * Add an attribute to this item.
 */
bool Item::add_attribute( Attribute* attribute )
{
    // Check if we should replace the attribute.
    size_t i;
    size_t length = attributes_.get_length();
    for (i = 0; i < length; ++i) {
		Attribute* current = attributes_.at( i );

		// Check if we should replace.
		if (attribute->get_index() == current->get_index()) {
			attributes_.at( i ) = attribute;
            JUTIL::BaseAllocator::destroy( current );
			return true;
		}
	}

	// Not overlapping, just append.
	return attributes_.push( attribute );
}

/*
 * Get total number of attributes this item has.
 */
size_t Item::get_attribute_count( void ) const
{
	size_t count = get_local_attribute_count();
	if (definition_ != nullptr) {
		count += definition_->get_attribute_count();
	}
    return count;
}

/*
 * Get number of local attributes this item has.
 */
size_t Item::get_local_attribute_count( void ) const
{
	size_t count = attributes_.get_length();
    return count;
}

/*
 * Get attribute from local array.
 */
Attribute* Item::get_local_attribute( size_t index ) const
{
	return attributes_.at( index );
}

/*
 * Get attribute by index in array.
 */
const Attribute* Item::get_attribute( size_t index ) const 
{
	size_t count = get_local_attribute_count();

	// If index is less than our local attribute length, we're
	// looking in definition length.
	if (index < count) {
		return get_local_attribute( index );
	}
	else {
		return definition_->get_attribute( index - count );
	}
}

/*
 * Checks for attribute by name in local and generic attributes.
 * Returns nullptr if not found.
 */
const Attribute* Item::find_attribute( const JUTIL::String* name ) const
{
    // Find attribute with matching index.
    size_t i;
    size_t length = get_attribute_count();
    for (i = 0; i < length; ++i) {
        const Attribute* attribute = get_attribute( i );
        const JUTIL::String* attribute_name = attribute->get_name();
        if (name->is_equal( attribute_name )) {
			return attribute;
		}
	}

	return nullptr;
}

/*
 * Set whether this item is selected.
 */
void Item::set_selected( bool selected )
{
	is_selected_ = selected;
}

/*
 * Return whether this item is selected.
 */
bool Item::is_selected( void ) const
{
	return is_selected_;
}

/*
 * Check if this status should replace existing and add it if so.
 */
bool Item::add_equipped_data( EEquipClass equip_class, EEquipSlot slot )
{
	// Check if we should replace the data.
    size_t i;
    size_t length = equipped_data_.get_length();
    for (i = 0; i < length; ++i) {
		EquippedStatus& current = equipped_data_.at( i );

		// Check if we should replace.
		if (equip_class == current.get_equip_class()) {
			current.set_equip_slot( slot );
		}
	}

	// Not overlapping, just append.
	EquippedStatus new_status( equip_class, slot );
	if (!equipped_data_.push( new_status )) {
		return false;
	}
	return true;
}

bool Item::remove_equipped_data( EEquipClass equip_class )
{
    size_t i;
    size_t length = equipped_data_.get_length();
    for (i = 0; i < length; ++i) {
		EquippedStatus& current = equipped_data_.at( i );

		// Check if we should remove.
		if (equip_class == current.get_equip_class()) {
			equipped_data_.erase( i );
			return true;
		}
	}

	// Not found
	return false;
}

size_t Item::get_equipped_count( void ) const
{
	return equipped_data_.get_length();
}

const EquippedStatus* Item::get_equipped_status( size_t index ) const
{
	return &equipped_data_.at( index );
}

const EquippedStatus* Item::find_equipped_status( EEquipClass equip_class ) const
{
    size_t i;
    size_t length = equipped_data_.get_length();
    for (i = 0; i < length; ++i) {
		const EquippedStatus* current = &equipped_data_.at( i );

		// Check if we should remove.
		if (current->get_equip_class() == equip_class) {
			return current;
		}
	}

	// Not found
	return nullptr;
}

bool Item::is_equipped( void ) const
{
	return get_equipped_count() > 0;
}

void Item::set_unique_id( uint64 unique_id )
{
	unique_id_ = unique_id;
}

void Item::set_type_index( uint16 typeIndex )
{
	type_index_ = typeIndex;
}

void Item::set_level( uint8 level )
{
	level_ = level;
}

void Item::set_quality( EItemQuality quality )
{
	quality_ = quality;
}

void Item::set_inventory_flags( uint32 inventory_flags )
{
	inventory_flags_ = inventory_flags;
}

void Item::set_count( uint32 count )
{
	count_ = count;
}

void Item::set_origin( uint32 origin )
{
	origin_ = origin;
}

void Item::set_flags( uint32 flags )
{
	flags_ = flags;
}