#include "Item.h"

const uint32 EQUIP_FLAGS[] = {
	0x00000000, //None
	0x00010000, //Scout
	0x00040000,
	0x00400000,
	0x00080000,
	0x00200000,
	0x01000000,
	0x00100000,
	0x00020000,
	0x00800000 //Spy
};

/* Create definitions Hashtable. */
Hashtable* Item::informationTable = NULL;

Item::Item(
	uint64 uniqueId,
	uint32 defIndex,
	uint32 level,
	EItemQuality quality,
	uint32 count,
	uint32 flags )
{
	// Set basic attributes.
	uniqueId_ = uniqueId;
	defIndex_ = defIndex;
	level_ = level;
	quality_ = quality;
	count_ = count;
	flags_ = flags;

	// Set null pointers for things to be acquired.
	texture_ = 0;
	name_ = 0;
	information_ = 0;

	// Load item's definition.
	loadInformation();

	// Reset selection and position.
	position_ = flags_ & 0xfff;
}

Item::~Item()
{
	//Item has been destroyed.
}

void Item::loadInformation()
{
	// Convert item type to string.
	string defIndex = boost::lexical_cast<string, int>( defIndex_ );

	try {
		information_ = informationTable->getTable( defIndex );
	}
	catch (Exception &) {
		// Failed to load definition; fall back to unknown.
		try {
			information_ = informationTable->getTable( "-1" );
		}
		catch (Exception &) {
			// Both failed.
			throw Exception( "Failed to get item information. No definition or default definition found." );
		}
	}

	// We've succesfully got information; set the texture.
	getTexture();
}

void Item::draw( DirectX* directX )
{
	directX->drawTexture( texture_, getX(), getY() );
}

uint64 Item::getUniqueId() const
{
	return uniqueId_;
}

uint32 Item::getDefIndex() const
{
	return defIndex_;
}

uint32 Item::getLevel() const
{
	return level_;
}

EItemQuality Item::getQuality() const
{
	return quality_;
}

uint32 Item::getFlags() const
{
	return flags_;
}

uint32 Item::getCount() const
{
	return count_;
}

uint16 Item::getIndex() const
{
	return position_;
}

string Item::getName()
{
	try {
		return *information_->getString( "name" );
	}
	catch (Exception &) {
		throw Exception( "Failed to get item name from table." );
	}
}

bool Item::isHat() const
{
	return getSlot() == "hat";
}

string Item::getSlot() const
{
	return *information_->getString( "slot" );
}

void Item::move( uint16 position )
{
	// Set to new position.
	position_ = position;

	// Reset item position.
	flags_ &= 0xfffff000;

	// Fix improper flags.
	if ((flags_ & 0xf0000000) != 0x80000000)
		flags_ = 0x80000000;

	// Put new position.
	flags_ += position_;
}

bool Item::isEquipped() const
{
	int equipFlags = flags_ & 0x0FFFF000;
	int validFlags = flags_ & 0xF0000000;
	return ((validFlags == 0x80000000) && (equipFlags != 0x00000000));
}

void Item::setEquip( int classIndex, bool equip )
{
	if (flags_ & EQUIP_FLAGS[classIndex]) {
		if (!equip)
			// Item is equipped to this class; remove flag.
			flags_ -= EQUIP_FLAGS[classIndex];
	}
	else {
		if (equip)
			// This item is not equipped to the class; add flag.
			flags_ += EQUIP_FLAGS[classIndex];
	}
}

Texture* Item::getTexture()
{
	if (texture_ == 0) {
		try	{
			texture_ = boost::any_cast<Texture*>( information_->get( "texture" ) );
		}
		catch (const boost::bad_any_cast &) {
			throw Exception( "Unexpected variable type for item texture." );
		}
	}
	
	return texture_;
}

int Item::getWidth() const
{
	return texture_->getWidth();
}

int Item::getHeight() const
{
	return texture_->getHeight();
}