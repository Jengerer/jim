#include "item_information.h"

ItemInformation::ItemInformation(
	const std::string& name,
	Texture *texture,
	uint32 classFlags,
	EItemSlot slot )
{
	SetName( name );
	set_texture( texture );
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

const std::string& ItemInformation::get_name( void ) const
{
	return name_;
}

const Texture* ItemInformation::get_texture( void ) const
{
	return texture_;
}

uint32 ItemInformation::get_class_flags( void ) const
{
	return classFlags_;
}

uint8 ItemInformation::get_class_count( void ) const
{
	return classCount_;
}

EItemSlot ItemInformation::get_slot( void ) const
{
	return slot_;
}

void ItemInformation::add_attribute( Attribute* attribute )
{
	attributes_.push_back( attribute );
}

size_t ItemInformation::get_attribute_count() const
{
	return attributes_.size();
}

const Attribute* ItemInformation::GetAttribute( size_t index ) const
{
	return attributes_[index];
}

void ItemInformation::SetName( const std::string& name )
{
	name_ = name;
}

void ItemInformation::set_texture( const Texture *texture )
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