#include "Item.h"

InformationMap *Item::definitions = nullptr;

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
	InformationMap::iterator i = definitions->find( GetTypeIndex() );
	if (i != definitions->end()) {
		information_ = i->second;
	}
	else {
		// Fall back to default.
		i = definitions->find( -1 );
		if (i != definitions->end()) {
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

const string& Item::GetName( void ) const
{
	return information_->GetName();
}

uint16 Item::GetIndex( void ) const
{
	return (GetFlags() & 0xFFFF) - 1;
}

void Item::SetIndex( uint16 position )
{
	// Remove new item flag.
	SetNew( false );

	uint32 tempFlags = GetFlags();
	tempFlags &= FL_ITEM_NONPOSITION; // Remove current position.
	tempFlags |= (position + 1) & FL_ITEM_POSITION; // Set new.
	SetFlags( tempFlags );
}

bool Item::IsNew( void ) const
{
	return GetFlags() & FL_ITEM_NEW;
}

void Item::SetNew( bool isNew )
{
	uint32 tempFlags = GetFlags();
	if (isNew) {
		SetFlags( tempFlags | FL_ITEM_NEW );
	}
	else {
		SetFlags( tempFlags & (FL_ITEM_ALL ^ FL_ITEM_NEW) );
	}
}

bool Item::HasValidFlags( void ) const
{
	return GetFlags() & FL_ITEM_VALID;
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
	return customName_.length() != 0;
}

bool Item::IsEquipped( EClassEquip equipClass ) const
{
	int equipFlags = flags_ & equipClass;
	int validFlags = flags_ & FL_ITEM_VALID;
	return ((validFlags != 0) && (equipFlags != 0x00000000));
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

void Item::SetEquip( EClassEquip equipClass, bool equip )
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