#include "item_information.h"

ItemInformation::ItemInformation(
	const string& name,
	Texture *texture,
	uint32 classFlags,
	EItemSlot slot )
{
	SetName( name );
	SetTexture( texture );
	SetClassFlags( classFlags );
	SetSlot( slot );
}

ItemInformation::~ItemInformation( void )
{
	// Item information destroyed.
	for (auto i = attributes_.begin(); i != attributes_.end(); i = attributes_.erase( i )) {
		delete *i;
	}
}

const string& ItemInformation::GetName( void ) const
{
	return name_;
}

const Texture* ItemInformation::GetTexture( void ) const
{
	return texture_;
}

uint32 ItemInformation::GetClassFlags( void ) const
{
	return classFlags_;
}

uint8 ItemInformation::GetClassCount( void ) const
{
	return classCount_;
}

EItemSlot ItemInformation::GetSlot( void ) const
{
	return slot_;
}

void ItemInformation::AddAttribute( Attribute* attribute )
{
	attributes_.push_back( attribute );
}

size_t ItemInformation::GetAttributeCount() const
{
	return attributes_.size();
}

const Attribute* ItemInformation::GetAttribute( size_t index ) const
{
	return attributes_[index];
}

void ItemInformation::SetName( const string& name )
{
	name_ = name;
}

void ItemInformation::SetTexture( const Texture *texture )
{
	texture_ = texture;
}

void ItemInformation::SetClassFlags( uint32 classFlags )
{
	classFlags_ = classFlags;

	// Now count the number of classes.
	classCount_ = 0;
	while (classFlags != 0) {
		if ((classFlags & 0x1) != 0) {
			classCount_++;
		}

		classFlags = classFlags >> 1;
	}
}

void ItemInformation::SetSlot( EItemSlot slot )
{
	slot_ = slot;
}