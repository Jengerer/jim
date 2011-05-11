#include "Item.h"

/* Create definitions Hashtable. */
Hashtable* Item::informationTable = NULL;

const int ITEM_SIZE		= 60;

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

	// Set constant size.
	SetSize( ITEM_SIZE, ITEM_SIZE );

	// Set null pointers for things to be acquired.
	texture_ = nullptr;
	information_ = nullptr;

	// Load item's definition.
	GetItemInformation();
}

Item::~Item( void )
{
	//Item has been destroyed.
}

void Item::GetItemInformation( void )
{
	// Convert item type to string.
	string defIndex = boost::lexical_cast<string, int>( GetTypeIndex() );
	try {
		information_ = informationTable->getTable( defIndex );
	}
	catch (Exception &) {
		try {
			information_ = informationTable->getTable( "-1" );
		}
		catch (Exception &) {
			throw Exception( "Failed to get item information. No definition or default definition found." );
		}
	}

	GetTexture();
}

void Item::OnDraw( DirectX* directX )
{
	directX->drawTexture( texture_, GetX(), GetY(), ITEM_SIZE, ITEM_SIZE );
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
	try {
		return *information_->getString( "name" );
	}
	catch (Exception &) {
		throw Exception( "Failed to get item name from table." );
	}
}

uint16 Item::GetIndex( void ) const
{
	return (GetFlags() & 0xFFF) - 1;
}

void Item::SetIndex( uint16 position )
{
	// Strip position from flags.
	uint32 tempFlags = GetFlags();
	tempFlags &= 0xFFFFF000;

	if ((tempFlags & 0x80000000) == 0) {
		tempFlags = 0x80000000;
	}

	tempFlags += position + 1;
	SetFlags( tempFlags );
}

bool Item::IsEquipped( EClassEquip equipClass ) const
{
	int equipFlags = flags_ & equipClass;
	int validFlags = flags_ & 0x80000000;
	return ((validFlags != 0) && (equipFlags != 0x00000000));
}

const string& Item::GetEquipSlot( void ) const
{
	return *information_->getString( "slot" );
}

Hashtable* Item::GetEquipClasses( void ) const
{
	if (information_->contains( "classes" )) {
		return information_->getTable( "classes" );
	}

	return nullptr;
}

void Item::SetEquip( EClassEquip equipClass, bool equip )
{
	if ((GetFlags() & equipClass) != 0) {
		if (!equip) {
			// Item is equipped to this class; remove flag.
			flags_ &= (0xffffffff - equipClass);
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
	if (texture_ == nullptr) {
		try	{
			texture_ = boost::any_cast<Texture*>( information_->get( "texture" ) );
		}
		catch (const boost::bad_any_cast &) {
			throw Exception( "Unexpected variable type for item texture." );
		}
	}
	
	return texture_;
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