#include "item.h"

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
	SetUniqueId( uniqueId );
	SetTypeIndex( typeIndex );
	SetLevel( level );
	SetQuality( quality );
	SetCount( count );
	SetFlags( flags );
	SetIndex( GetPosition() );
	SetOrigin( origin );

	// Get item information.
	GetItemInformation();
}

Item::~Item( void )
{
	// Remove all attributes.
	for (auto i = attributes_.begin(); i != attributes_.end(); i = attributes_.erase( i )) {
		delete *i;
	}
}

void Item::GetItemInformation( void )
{
	// Attempt to find item information.
	InformationMap::iterator i = definitions.find( GetTypeIndex() );
	if (i != definitions.end()) {
		information_ = i->second;
	}
	else {
		information_ = fallback;
	}

	// Load attributes from class definition.
	for (size_t i = 0, len = information_->GetAttributeCount(); i < len; ++i) {
		const Attribute* orig = information_->GetAttribute( i );
		add_attribute( new Attribute( *orig ) );
	}
}

uint64 Item::GetUniqueId( void ) const
{
	return uniqueId_;
}

uint16 Item::GetTypeIndex( void ) const
{
	return typeIndex_;
}

uint8 Item::GetLevel( void ) const
{
	return level_;
}

EItemQuality Item::GetQuality( void ) const
{
	return quality_;
}

uint32 Item::GetFlags( void ) const
{
	return flags_;
}

uint32 Item::GetCount( void ) const
{
	return count_;
}

uint32 Item::GetOrigin( void ) const
{
	return origin_;
}

std::string Item::GetName( void ) const
{
	if (HasCustomName()) {
		return '"' + GetCustomName() + '"';
	}
	else {
		std::string itemName = information_->GetName();
		std::string qualityName = GetQualityName();
		if (!qualityName.empty()) {
			std::string fullPrefix = qualityName + ' ';
			itemName.insert( itemName.begin(), fullPrefix.begin(), fullPrefix.end() );
		}

		return itemName;
	}
}

const Colour& Item::GetQualityColour( void ) const
{
	switch (GetQuality()) {
	case k_EItemQuality_Vintage:
		return QUALITY_VINTAGE_COLOUR;
		break;

	case k_EItemQuality_Genuine:
		return QUALITY_GENUINE_COLOUR;
		break;

	case k_EItemQuality_Unusual:
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

const char* Item::GetQualityName( void ) const
{
	switch (GetQuality()) {
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

	default:
		return "";
		break;
	}
}

uint16 Item::GetPosition( void ) const
{
	return (GetFlags() & 0xFFFF) - 1;
}

void Item::SetPosition( uint16 position )
{
	uint32 tempFlags = HasValidFlags() ? GetFlags() : FL_ITEM_VALID;
	tempFlags &= FL_ITEM_NONPOSITION; // Remove current position.
	tempFlags |= (position + 1) & FL_ITEM_POSITION; // Set new.
	SetFlags( tempFlags );
}

uint32 Item::GetIndex( void ) const
{
	return index_;
}

void Item::SetIndex( uint32 index )
{
	index_ = index;
}

bool Item::HasValidFlags( void ) const
{
	return (GetFlags() & 0xF0000000) == FL_ITEM_VALID;
}

void Item::SetCustomName( const std::string& name )
{
	customName_ = '"' + name + '"';
}

const std::string& Item::GetCustomName( void ) const
{
	return customName_;
}

bool Item::HasCustomName( void ) const
{
	return !customName_.empty();
}

bool Item::IsEquipped( uint32 equipClass ) const
{
	int equipFlags = flags_ & equipClass;
	return HasValidFlags() && (equipFlags != 0);
}

bool Item::ClassUses( uint32 classFlags ) const
{
	return (GetEquipClasses() & classFlags) != 0;
}

EItemSlot Item::GetEquipSlot( void ) const
{
	return information_->GetSlot();
}

uint32 Item::GetEquipClasses( void ) const
{
	return information_->GetClassFlags();
}

uint8 Item::GetEquipClassCount( void ) const
{
	return information_->GetClassCount();
}

void Item::SetEquip( uint32 equipClass, bool equip )
{
	if ((GetFlags() & equipClass) != 0) {
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

const Texture* Item::GetTexture( void )
{
	return information_->GetTexture();
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

void Item::SetUniqueId( uint64 uniqueId )
{
	uniqueId_ = uniqueId;
}

void Item::SetTypeIndex( uint16 typeIndex )
{
	typeIndex_ = typeIndex;
}

void Item::SetLevel( uint8 level )
{
	level_ = level;
}

void Item::SetQuality( EItemQuality quality )
{
	quality_ = quality;
}

void Item::SetFlags( uint32 flags )
{
	flags_ = flags;
}

void Item::SetCount( uint32 count )
{
	count_ = count;
}

void Item::SetOrigin( uint32 origin )
{
	origin_ = origin;
}