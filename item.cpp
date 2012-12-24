#include "item.hpp"
#include <jui/application/error_stack.hpp>

// TODO: Should change attribute iteration so we don't have to make copies.
// Just iterate through both local and item attributes, and get from appropriate group.

const JUTIL::ConstantString CANNOT_TRADE_NAME = "cannot trade";
const JUTIL::ConstantString ALWAYS_TRADE_NAME = "always tradable";

const JUTIL::ConstantString CRATE_ATTRIBUTE_NAME = "set supply crate series";
const JUTIL::ConstantString PAINT_ATTRIBUTE_NAME_0 = "set item tint RGB";
const JUTIL::ConstantString PAINT_ATTRIBUTE_NAME_1 = "set item tint RGB 2";

const JUTIL::ConstantString STRANGE_ATTRIBUTE_NAME_0 = "kill eater";
const JUTIL::ConstantString STRANGE_ATTRIBUTE_NAME_1 = "kill eater 2";
const JUTIL::ConstantString STRANGE_ATTRIBUTE_NAME_2 = "kill eater user 1";
const JUTIL::ConstantString STRANGE_ATTRIBUTE_NAME_3 = "kill eater user 2";
const JUTIL::ConstantString STRANGE_ATTRIBUTE_NAME_4 = "kill eater user 3";

const JUTIL::ConstantString STRANGE_TYPE_NAME_0 = "kill eater score type";
const JUTIL::ConstantString STRANGE_TYPE_NAME_1 = "kill eater score type 2";
const JUTIL::ConstantString STRANGE_TYPE_NAME_2 = "kill eater user score type 1";
const JUTIL::ConstantString STRANGE_TYPE_NAME_3 = "kill eater user score type 2";
const JUTIL::ConstantString STRANGE_TYPE_NAME_4 = "kill eater user score type 3";


/*
 * Item constructor from attributes.
 */
Item::Item(
	uint64 unique_id,
	uint16 type_index,
	uint8 level,
	EItemQuality quality,
	uint32 count,
	uint32 flags,
	uint32 origin )
{
	// Set basic attributes.
	set_unique_id( unique_id );
	set_type_index( type_index );
	set_level( level );
	set_quality( quality );
	set_count( count );
	set_flags( flags );
	set_index( get_position() );
	set_origin( origin );
	set_renamed( false );
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
        Attribute* attribute = attributes_.get( i );
        JUTIL::BaseAllocator::destroy( attribute );
    }
    attributes_.clear();
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
		uint32 quality_num = static_cast<uint32>(quality_attrib->get_value().as_float);
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
 * Get flags for item.
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
	return count_;
}

/*
 * Get item origin.
 */
uint32 Item::get_origin( void ) const
{
	return origin_;
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
	case k_EItemQuality_Haunted:
		return &QUALITY_UNUSUAL_COLOUR;
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
	return (get_flags() & FL_ITEM_POSITION) - 1;
}

/*
 * Set item new position index.
 * Position indices are considered to start at 0 and end at one less than index count.
 */
void Item::set_position( uint16 position )
{
	uint32 tempFlags = has_valid_flags() ? get_flags() : FL_ITEM_VALID;
	tempFlags &= FL_ITEM_NONPOSITION; // Remove current position.
	tempFlags |= (position + 1) & FL_ITEM_POSITION; // Set new.
	set_flags( tempFlags );
}

/*
 * Get item slot index.
 */
uint32 Item::get_index( void ) const
{
	return index_;
}

/*
 * Set item slot index.
 */
void Item::set_index( uint32 index )
{
	index_ = index;
}

/*
 * Return whether this item containts valid flags.
 */
bool Item::has_valid_flags( void ) const
{
	return (get_flags() & 0xF0000000) == FL_ITEM_VALID;
}

/* Checks whether an item is allowed to be traded. */
bool Item::is_tradable( void ) const
{
	return ((get_origin() != ORIGIN_ACHIEVEMENT) &&
		(find_attribute( &CANNOT_TRADE_NAME ) == nullptr)) ||
		(find_attribute( &ALWAYS_TRADE_NAME ) != nullptr);
}

/* Checks whether this item is equipped. */
bool Item::is_equipped( uint32 equip_class ) const
{
	int equipFlags = flags_ & equip_class;
	return has_valid_flags() && (equipFlags != 0);
}

/* Checks if the equip flags match this item. */
bool Item::class_uses( uint32 class_flags ) const
{
	return (get_equip_classes() & class_flags) != 0;
}

/* Gets the slot this item equips to. */
EItemSlot Item::get_equip_slot( void ) const
{
	return definition_->get_slot();
}

/* Gets all item equip flags. */
uint32 Item::get_equip_classes( void ) const
{
	return definition_->get_class_flags();
}

/*
 * Get number of classes that this item can be equipped on.
 */
uint8 Item::get_equip_class_count( void ) const
{
	return definition_->get_class_count();
}

/*
 * Set the class bit-vector that this item is equipped on.
 */
void Item::set_equip( uint32 equip_class, bool equip )
{
	if ((get_flags() & equip_class) != 0) {
		if (!equip) {
			// Item is equipped to this class; remove flag.
			flags_ &= (FL_ITEM_ALL ^ equip_class);
		}
	}
	else {
		if (equip) {
			// This item is not equipped to the class; add flag.
			flags_ |= equip_class;
		}
	}
}

/*
 * Gets the crate number. Returns FL_ITEM_NOT_CRATE if not a crate
 */
uint32 Item::get_crate_number( void ) const
{
	const Attribute* attribute = find_attribute(&CRATE_ATTRIBUTE_NAME);
	if(attribute == nullptr){
		return FL_ITEM_NOT_CRATE;
	}
	return (uint32) attribute->get_value().as_float;
}

/*
 * Gets the paint value as 0x00RRGGBB. Returns FL_ITEM_NOT_PAINTED if not painted
 */
uint32 Item::get_paint_value( uint32 index ) const
{
	const Attribute* attribute;
	switch (index) {
	case 0:
		attribute = find_attribute(&PAINT_ATTRIBUTE_NAME_0);
		break;

	case 1:
		attribute = find_attribute(&PAINT_ATTRIBUTE_NAME_1);
		break;

	default:
		//should probably be some sort of INVALID_INDEX flag somewhere
		return FL_ITEM_NOT_PAINTED;
	}
	if(attribute == nullptr){
		return FL_ITEM_NOT_PAINTED;
	}
	if(index == 1){
		if((uint32) attribute->get_value().as_float == get_paint_value(0)){
			return FL_ITEM_NOT_PAINTED;
		}
	}
	return (uint32) attribute->get_value().as_float;
}

/*
 * Gets the craft number.  Returns 0 if no craft number is avaliable.
 */
uint32 Item::get_craft_number( void ) const
{
	const JUTIL::ConstantString CRAFT_ATTRIBUTE_NAME = "unique craft index";
	const Attribute* attribute = find_attribute(&CRAFT_ATTRIBUTE_NAME);
	if(attribute == nullptr){
		return 0;
	}
	return attribute->get_value().as_uint32;
}

/*
 * Gets the strange number.  Returns FL_ITEM_NOT_STRANGE if no strange number is avaliable.
 * 
 * Passing in 0 will fetch the first strange kill count
 * Passing in 1 will fetch the alternate kill count
 * Passing in 2-4 will fetch the strange part kill counts
 */
uint32 Item::get_strange_number( uint32 index ) const
{
	const Attribute* attribute;
	switch (index) {
	case 0:
		attribute = find_attribute(&STRANGE_ATTRIBUTE_NAME_0);
		break;

	case 1:
		attribute = find_attribute(&STRANGE_ATTRIBUTE_NAME_1);
		break;

	case 2:
		attribute = find_attribute(&STRANGE_ATTRIBUTE_NAME_2);
		break;

	case 3:
		attribute = find_attribute(&STRANGE_ATTRIBUTE_NAME_3);
		break;

	case 4:
		attribute = find_attribute(&STRANGE_ATTRIBUTE_NAME_4);
		break;

	default:
		//should probably be some sort of INVALID_INDEX flag somewhere
		return FL_ITEM_NOT_STRANGE;
	}
	if(attribute == nullptr){
		return FL_ITEM_NOT_STRANGE;
	}
	return attribute->get_value().as_uint32;
}

/*
 * Gets the type of strange number.  Returns 0 if no strange type data is avaliable.
 * 
 * Passing in 0 will fetch the first strange kill count
 * Passing in 1 will fetch the alternate kill count
 * Passing in 2-4 will fetch the strange part kill counts
 */
uint32 Item::get_strange_type( uint32 index ) const
{
	const Attribute* attribute;
	switch (index) {
	case 0:
		attribute = find_attribute(&STRANGE_TYPE_NAME_0);
		break;

	case 1:
		attribute = find_attribute(&STRANGE_TYPE_NAME_1);
		break;

	case 2:
		attribute = find_attribute(&STRANGE_TYPE_NAME_2);
		break;

	case 3:
		attribute = find_attribute(&STRANGE_TYPE_NAME_3);
		break;

	case 4:
		attribute = find_attribute(&STRANGE_TYPE_NAME_4);
		break;

	default:
		//should probably be some sort of INVALID_INDEX flag somewhere
		return 0;
	}
	if(attribute == nullptr){
		return 0;
	}
	return (uint32) attribute->get_value().as_float;
}

/*
 * Get a pointer to the texture for this item, if any.
 */
const JUI::Texture* Item::get_texture( void )
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
		Attribute* current = attributes_.get( i );

		// Check if we should replace.
		if (attribute->get_index() == current->get_index()) {
			attributes_.set( i, attribute );
            JUTIL::BaseAllocator::destroy( current );
			return true;
		}
	}

	// Not overlapping, just append.
	return attributes_.push( attribute );
}

/*
 * Get number of local attributes this item has.
 */
size_t Item::get_attribute_count( void ) const
{
    return attributes_.get_length();
}

/*
 * Get local attribute by index in array.
 */
Attribute* Item::get_attribute( size_t index )
{
    return attributes_.get( index );
}

/*
 * Get constant local attribute by index in array.
 */
const Attribute* Item::get_attribute( size_t index ) const
{
    return attributes_.get( index );
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

	// Not found in local attributes, check definition.
	return definition_->find_attribute( name );
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

void Item::set_flags( uint32 flags )
{
	flags_ = flags;
}

void Item::set_count( uint32 count )
{
	count_ = count;
}

void Item::set_origin( uint32 origin )
{
	origin_ = origin;
}