#include "item.hpp"
#include <jui/application/error_stack.hpp>

// TODO: Should change attribute iteration so we don't have to make copies.
// Just iterate through both local and item attributes, and get from appropriate group.

// Item quality names.
// TODO: I think somebody mentioned these quality names are available elsewhere.
const JUTIL::ConstantString VINTAGE_QUALITY_NAME = "Vintage";
const JUTIL::ConstantString GENUINE_QUALITY_NAME = "Genuine";
const JUTIL::ConstantString UNUSUAL_QUALITY_NAME = "Unusual";
const JUTIL::ConstantString STRANGE_QUALITY_NAME = "Strange";
const JUTIL::ConstantString COMMUNITY_QUALITY_NAME = "Community";
const JUTIL::ConstantString SELF_MADE_QUALITY_NAME = "Self-Made";
const JUTIL::ConstantString VALVE_QUALITY_NAME = "Valve";
const JUTIL::ConstantString HAUNTED_QUALITY_NAME = "Haunted";


const JUTIL::ConstantString DROPPED_ORIGIN_NAME = "Timed Drop";					//0
const JUTIL::ConstantString ACHIEVEMENT_ORIGIN_NAME = "Achievement";			//1
const JUTIL::ConstantString PURCHASED_ORIGIN_NAME = "Purchased";				//2
const JUTIL::ConstantString TRADED_ORIGIN_NAME = "Traded";						//3
const JUTIL::ConstantString CRAFTED_ORIGIN_NAME = "Crafted";					//4
const JUTIL::ConstantString STORE_ORIGIN_NAME = "Store Promotion";				//5
const JUTIL::ConstantString GIFTED_ORIGIN_NAME = "Gifted";						//6
const JUTIL::ConstantString SUPPORT_ORIGIN_NAME = "Support Granted";			//7
const JUTIL::ConstantString CRATE_ORIGIN_NAME = "Found in a Crate";				//8
const JUTIL::ConstantString EARNED_ORIGIN_NAME = "Earned";						//9
const JUTIL::ConstantString THIRDPARTY_ORIGIN_NAME = "Third-Party Promotion";	//10
const JUTIL::ConstantString WRAPPED_ORIGIN_NAME = "Wrapped Gift";				//11
const JUTIL::ConstantString HALLOWEEN_ORIGIN_NAME = "Halloween Drop";			//12
const JUTIL::ConstantString STEAM_ORIGIN_NAME = "Steam Purchase";				//13
const JUTIL::ConstantString FOREIGN_ORIGIN_NAME = "Foreign Item";				//14
const JUTIL::ConstantString CD_ORIGIN_NAME = "CD Key";							//15
const JUTIL::ConstantString REWARD_ORIGIN_NAME = "Collection Reward";			//16
const JUTIL::ConstantString PREVIEW_ORIGIN_NAME = "Preview Item";				//17
const JUTIL::ConstantString WORKSHOP_ORIGIN_NAME = "Workshop Contribution";		//18
const JUTIL::ConstantString AWARDED_ORIGIN_NAME = "Periodic Score Reward";		//19
const JUTIL::ConstantString MVMBADGE_ORIGIN_NAME = "MvM Badge completion reward";//20
const JUTIL::ConstantString MVMSURPLUS_ORIGIN_NAME = "MvM Squad surplus reward";//21


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
	return true;
}

/*
 * Resolve item name from definitions.
 */
bool Item::generate_name( void )
{
	// Error stack for logging.
	JUI::ErrorStack* stack = JUI::ErrorStack::get_instance();

    // Write string.
    const JUTIL::String* quality_name = get_quality_name();
    if (quality_name != nullptr) {
        if (!item_name_.write( "%s ", quality_name->get_string() )) {
			stack->log( "Failed to write item quality name when generating name string." );
            return false;
        }
    }

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
 * Get quality name.
 */
const JUTIL::String* Item::get_quality_name( void ) const
{
	switch (get_quality()) {
	case k_EItemQuality_Vintage:
		return &VINTAGE_QUALITY_NAME;
		break;

	case k_EItemQuality_Genuine:
		return &GENUINE_QUALITY_NAME;
		break;

	case k_EItemQuality_Unusual:
		return &UNUSUAL_QUALITY_NAME;
		break;

	case k_EItemQuality_Strange:
		return &STRANGE_QUALITY_NAME;
		break;

	case k_EItemQuality_Community:
		return &COMMUNITY_QUALITY_NAME;
		break;

	case k_EItemQuality_SelfMade:
		return &SELF_MADE_QUALITY_NAME;
		break;

	case k_EItemQuality_Valve:
		return &VALVE_QUALITY_NAME;
		break;

	case k_EItemQuality_Haunted:
		return &HAUNTED_QUALITY_NAME;
		break;

	default:
		return nullptr;
		break;
	}
}

/*
 * Get origin name.
 */
const JUTIL::String* Item::get_origin_name( void ) const
{
	switch (get_origin()) {
	case k_EItemOrigin_Dropped:
		return &DROPPED_ORIGIN_NAME;
		break;

	case k_EItemOrigin_Achievement:
		return &ACHIEVEMENT_ORIGIN_NAME;
		break;

	case k_EItemOrigin_Purchased:
		return &PURCHASED_ORIGIN_NAME;
		break;

	case k_EItemOrigin_Traded:
		return &TRADED_ORIGIN_NAME;
		break;

	case k_EItemOrigin_Crafted:
		return &CRAFTED_ORIGIN_NAME;
		break;

	case k_EItemOrigin_Store:
		return &STORE_ORIGIN_NAME;
		break;

	case k_EItemOrigin_Gifted:
		return &GIFTED_ORIGIN_NAME;
		break;

	case k_EItemOrigin_Support:
		return &SUPPORT_ORIGIN_NAME;
		break;

	case k_EItemOrigin_Crate:
		return &CRATE_ORIGIN_NAME;
		break;

	case k_EItemOrigin_Earned:
		return &EARNED_ORIGIN_NAME;
		break;

	case k_EItemOrigin_ThirdParty:
		return &THIRDPARTY_ORIGIN_NAME;
		break;

	case k_EItemOrigin_Wrapped:
		return &WRAPPED_ORIGIN_NAME;
		break;

	case k_EItemOrigin_Halloween:
		return &HALLOWEEN_ORIGIN_NAME;
		break;

	case k_EItemOrigin_Steam:
		return &STEAM_ORIGIN_NAME;
		break;

	case k_EItemOrigin_Foreign:
		return &FOREIGN_ORIGIN_NAME;
		break;

	case k_EItemOrigin_CD:
		return &CD_ORIGIN_NAME;
		break;

	case k_EItemOrigin_Reward:
		return &REWARD_ORIGIN_NAME;
		break;

	case k_EItemOrigin_Preview:
		return &PREVIEW_ORIGIN_NAME;
		break;

	case k_EItemOrigin_Workshop:
		return &WORKSHOP_ORIGIN_NAME;
		break;

	case k_EItemOrigin_Awarded:
		return &AWARDED_ORIGIN_NAME;
		break;

	case k_EItemOrigin_MvMBadge:
		return &MVMBADGE_ORIGIN_NAME;
		break;

	case k_EItemOrigin_MvMSurplus:
		return &MVMSURPLUS_ORIGIN_NAME;
		break;

	default:
		return nullptr;
		break;
	}

}

/*
 * Get item position index.
 * Position indices are considered to start at 0 and end at one less than index count.
 */
uint16 Item::get_position( void ) const
{
	return (get_flags() & 0xFFFF) - 1;
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
    const JUTIL::ConstantString CANNOT_TRADE_NAME = "cannot trade";
    const JUTIL::ConstantString ALWAYS_TRADE_NAME = "always tradable";
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