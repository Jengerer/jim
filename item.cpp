#include "item.hpp"

InformationMap Item::definitions;
AttributeMap Item::attributes;
ItemInformation* Item::fallback = nullptr;

Item::Item(
	uint64 uniqueId,
	uint16 typeIndex,
	uint8 level,
	EItemQuality quality,
	uint32 count,
	uint32 flags,
	uint32 origin )
{
	// Set basic attributes.
	set_unique_id( uniqueId );
	set_type_index( typeIndex );
	set_level( level );
	set_quality( quality );
	set_count( count );
	set_flags( flags );
	set_index( get_position() );
	set_origin( origin );

	// Get item information.
	get_item_information();
}

Item::~Item( void )
{
	// Remove all attributes.
	for (auto i = attributes_.begin(); i != attributes_.end(); i = attributes_.erase( i )) {
		delete *i;
	}
}

void Item::get_item_information( void )
{
	// Attempt to find item information.
    ItemInformation* information;
    if (definitions.get( get_type_index(), &information )) {
        information_ = information;
    }
    else {
        information_ = fallback;
    }

	// Load attributes from class definition.
    size_t i;
    size_t len = information_->get_attribute_count();
	for (size_t i = 0; i < len; ++i) {
		const Attribute* orig = information_->get_attribute( i );
		add_attribute( new Attribute( *orig ) );
	}
}

void Item::update_attributes( void )
{
	// Check if quality is elevated.
	const Attribute* quality_attrib = get_attribute_by_name( "elevated quality" );
	if (quality_attrib != nullptr) {
		// First get quality number.
		uint32 quality_num = static_cast<uint32>(quality_attrib->get_float_value());
		set_quality( static_cast<EItemQuality>(quality_num) );
	}
}

uint64 Item::get_unique_id( void ) const
{
	return uniqueId_;
}

uint16 Item::get_type_index( void ) const
{
	return typeIndex_;
}

uint8 Item::get_level( void ) const
{
	return level_;
}

EItemQuality Item::get_quality( void ) const
{
	return quality_;
}

uint32 Item::get_flags( void ) const
{
	return flags_;
}

uint32 Item::get_count( void ) const
{
	return count_;
}

uint32 Item::get_origin( void ) const
{
	return origin_;
}

std::string Item::get_name( void ) const
{
	if (has_custom_name()) {
		return '"' + get_custom_name() + '"';
	}
	else {
		std::string itemName = information_->get_name();
		std::string qualityName = get_quality_name();
		if (!qualityName.empty()) {
			std::string fullPrefix = qualityName + ' ';
			itemName.insert( itemName.begin(), fullPrefix.begin(), fullPrefix.end() );
		}

		return itemName;
	}
}

const JUI::Colour& Item::get_quality_colour( void ) const
{
	switch (get_quality()) {
	case k_EItemQuality_Vintage:
		return QUALITY_VINTAGE_COLOUR;
		break;

	case k_EItemQuality_Genuine:
		return QUALITY_GENUINE_COLOUR;
		break;

	case k_EItemQuality_Unusual:
	case k_EItemQuality_Haunted:
		return QUALITY_UNUSUAL_COLOUR;
		break;

	case k_EItemQuality_Strange:
		return QUALITY_STRANGE_COLOUR;
		break;

	case k_EItemQuality_Community:
	case k_EItemQuality_SelfMade:
		return QUALITY_COMMUNITY_COLOUR;
		break;

	case k_EItemQuality_Valve:
		return QUALITY_VALVE_COLOUR;
		break;

	default:
		return QUALITY_COMMON_COLOUR;
		break;
	}
}

const char* Item::get_quality_name( void ) const
{
	switch (get_quality()) {
	case k_EItemQuality_Vintage:
		return "Vintage";
		break;

	case k_EItemQuality_Genuine:
		return "Genuine";
		break;

	case k_EItemQuality_Unusual:
		return "Unusual";
		break;

	case k_EItemQuality_Strange:
		return "Strange";
		break;

	case k_EItemQuality_Community:
		return "Community";
		break;

	case k_EItemQuality_SelfMade:
		return "Self-Made";
		break;

	case k_EItemQuality_Valve:
		return "Valve";
		break;

	case k_EItemQuality_Haunted:
		return "Haunted";
		break;

	default:
		return "";
		break;
	}
}

uint16 Item::get_position( void ) const
{
	return (get_flags() & 0xFFFF) - 1;
}

void Item::set_position( uint16 position )
{
	uint32 tempFlags = has_valid_flags() ? get_flags() : FL_ITEM_VALID;
	tempFlags &= FL_ITEM_NONPOSITION; // Remove current position.
	tempFlags |= (position + 1) & FL_ITEM_POSITION; // Set new.
	set_flags( tempFlags );
}

uint32 Item::get_index( void ) const
{
	return index_;
}

void Item::set_index( uint32 index )
{
	index_ = index;
}

bool Item::has_valid_flags( void ) const
{
	return (get_flags() & 0xF0000000) == FL_ITEM_VALID;
}

void Item::set_custom_name( const std::string& name )
{
	customName_ = '"' + name + '"';
}

const std::string& Item::get_custom_name( void ) const
{
	return customName_;
}

bool Item::has_custom_name( void ) const
{
	return !customName_.empty();
}

/* Checks whether an item is allowed to be traded. */
bool Item::is_tradable( void ) const
{
	return ((get_origin() != ORIGIN_ACHIEVEMENT) &&
		(get_attribute_by_name( "cannot trade" ) == nullptr)) ||
		(get_attribute_by_name( "always tradable" ) != nullptr);
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

uint8 Item::get_equip_class_count( void ) const
{
	return information_->get_class_count();
}

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

const JUI::Texture* Item::get_texture( void )
{
	return information_->get_texture();
}

void Item::add_attribute( Attribute* attribute )
{
	for (auto i = attributes_.begin(); i != attributes_.end(); ++i) {
		Attribute* current = *i;

		// Check if we should replace.
		if (attribute->get_index() == current->get_index()) {
			*i = attribute;
			delete current;
			return;
		}
	}

	// Not overlapping, just append.
	attributes_.push_back( attribute );
}

size_t Item::get_attribute_count() const
{
	return attributes_.size();
}

const Attribute* Item::get_attribute_at( size_t index ) const
{
	return attributes_.at( index );
}

const Attribute* Item::get_attribute_by_index( size_t index ) const
{
	for each (Attribute* i in attributes_) {
		if (i->get_index() == index) {
			return i;
		}
	}

	return nullptr;
}

const Attribute* Item::get_attribute_by_name( const std::string& name ) const
{
	for each (Attribute* i in attributes_) {
		if (i->get_name() == name) {
			return i;
		}
	}

	return nullptr;
}

void Item::set_unique_id( uint64 uniqueId )
{
	uniqueId_ = uniqueId;
}

void Item::set_type_index( uint16 typeIndex )
{
	typeIndex_ = typeIndex;
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