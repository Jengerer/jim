#include "item.h"

// Static map.
InformationMap	Item::definitions;
AttributeMap	Item::attributes;

Item::Item(
	uint64 uniqueId,
	uint16 typeIndex,
	uint8 level,
	EItemQuality quality,
	uint32 count,
	uint32 flags )
{
	// Set basic attributes.
	SetUniqueId( uniqueId );
	SetTypeIndex( typeIndex );
	SetLevel( level );
	SetQuality( quality );
	SetCount( count );
	SetFlags( flags );
	SetIndex( GetPosition() );

	// Set null pointers for things to be acquired.
	information_ = nullptr;
	GetItemInformation();
}

Item::~Item( void )
{
	//Item has been destroyed.
}

void Item::GetItemInformation( void )
{
	// Attempt to find item information.
	InformationMap::iterator i = definitions.find( GetTypeIndex() );
	if (i != definitions.end()) {
		information_ = i->second;
	}
	else {
		// Fall back to default.
		i = definitions.find( -1 );
		if (i != definitions.end()) {
			information_ = i->second;
		}
		else {
			throw Exception( "Failed to find item or fallback definition." );
		}
	}

	GetTexture();
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

string Item::GetName( void ) const
{
	if (HasCustomName()) {
		return '"' + GetCustomName() + '"';
	}
	else {
		string itemName = information_->GetName();
		string qualityName = GetQualityName();
		if (!qualityName.empty()) {
			string fullPrefix = qualityName + ' ';
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

void Item::SetCustomName( const string& name )
{
	customName_ = '"' + name + '"';
}

const string& Item::GetCustomName( void ) const
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

Texture* Item::GetTexture( void )
{
	return information_->GetTexture();
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