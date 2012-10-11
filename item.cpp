#include "item.hpp"

InformationMap Item::definitions;
AttributeMap Item::attributes;
ItemInformation* Item::fallback = nullptr;

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
 * Find the item information given the type.
 *
 * Parameters:
 * - custom_name : A pointer to a custom name string if one exists, or nullptr if none.
 */
bool Item::update_item_information( JUTIL::String* custom_name )
{
	// Attempt to find item information.
    ItemInformation* information;
    if (definitions.get( get_type_index(), &information )) {
        information_ = information;
    }
    else {
        information_ = fallback;
    }

    // Generate full item name.
    if (custom_name != nullptr) {
        item_name_ = custom_name;
    }
    else {
        // Put quality and item name together.
        JUTIL::DynamicString* name;
        if (!JUTIL::BaseAllocator::allocate( &name )) {
            return false;
        }
        name = new (name) JUTIL::DynamicString();

        // Write string.
        const JUTIL::String* quality_name = get_quality_name();
        if (!name->write( "%s %s", quality_name, information_->get_name() )) {
            JUTIL::BaseAllocator::destroy( name );
            return false;
        }
    }

    // TODO: Should change attribute iteration so we don't have to make copies.
    // Just iterate through both local and item attributes, and get from appropriate group.
    return true;
}

/*
 * Update item attributes based on attribute list.
 */
void Item::update_attributes( void )
{
	// Check if quality is elevated.
    const JUTIL::ConstantString ELEVATED_QUALITY_NAME = "elevated quality";
	const Attribute* quality_attrib = get_attribute_by_name( &ELEVATED_QUALITY_NAME );
	if (quality_attrib != nullptr) {
		// First get quality number.
		uint32 quality_num = static_cast<uint32>(quality_attrib->get_float_value());
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
	// Name should be generated in item.
    return nullptr;
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
		return &VALVE_QUALITY_NAME;
		break;

	default:
        JUTIL::JUTILBase::debug_assert( false );
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
		(get_attribute_by_name( &CANNOT_TRADE_NAME ) == nullptr)) ||
		(get_attribute_by_name( &ALWAYS_TRADE_NAME ) != nullptr);
}

/* Checks whether this item is equipped. */
bool Item::is_equipped( uint32 equipClass ) const
{
	int equipFlags = flags_ & equipClass;
	return has_valid_flags() && (equipFlags != 0);
}

/* Checks if the equip flags match this item. */
bool Item::class_uses( uint32 classFlags ) const
{
	return (get_equip_classes() & classFlags) != 0;
}

/* Gets the slot this item equips to. */
EItemSlot Item::get_equip_slot( void ) const
{
	return information_->get_slot();
}

/* Gets all item equip flags. */
uint32 Item::get_equip_classes( void ) const
{
	return information_->get_class_flags();
}

/*
 * Get number of classes that this item can be equipped on.
 */
uint8 Item::get_equip_class_count( void ) const
{
	return information_->get_class_count();
}

/*
 * Set the class bit-vector that this item is equipped on.
 */
void Item::set_equip( uint32 equipClass, bool equip )
{
	if ((get_flags() & equipClass) != 0) {
		if (!equip) {
			// Item is equipped to this class; remove flag.
			flags_ &= (FL_ITEM_ALL ^ equipClass);
		}
	}
	else {
		if (equip) {
			// This item is not equipped to the class; add flag.
			flags_ |= equipClass;
		}
	}
}

/*
 * Get a pointer to the texture for this item, if any.
 */
const JUI::Texture* Item::get_texture( void )
{
	return information_->get_texture();
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
 * Get number of attributes this item has.
 */
size_t Item::get_attribute_count( void ) const
{
    // TODO: Return this number plus the item class attributes.
	return attributes_.get_length();
}

/*
 * Get attribute by index.
 */
const Attribute* Item::get_attribute_at( size_t index ) const
{
	return attributes_.get( index );
}

/*
 * Get attribute by attribute index, if any. Returns nullptr if not found.
 */
const Attribute* Item::get_attribute_by_index( size_t index ) const
{
    // Find attribute with matching index.
    size_t i;
    size_t length = attributes_.get_length();
    for (i = 0; i < length; ++i) {
        Attribute* attribute = attributes_.get( i );
        if (attribute->get_index() == index) {
			return attribute;
		}
	}

    // Not found.
	return nullptr;
}

/*
 * Find attribute by name.
 */
const Attribute* Item::get_attribute_by_name( const JUTIL::String* name ) const
{
    // Find attribute with equal name.
    size_t i;
    size_t length = attributes_.get_length();
    for (i = 0; i < length; ++i) {
        Attribute* attribute = attributes_.get( i );
        const JUTIL::String* attribute_name = attribute->get_name();
        if (attribute_name->is_equal( name )) {
            return attribute;
        }
    }

	return nullptr;
}

void Item::set_unique_id( uint64 uniqueId )
{
	unique_id_ = uniqueId;
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