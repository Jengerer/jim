#include "item_information.hpp"

ItemInformation::ItemInformation(
	JUTIL::String* name,
	JUI::Texture* texture,
	uint32 classFlags,
	EItemSlot slot )
{
	set_name( name );
	set_texture( texture );
	set_class_flags( classFlags );
	set_slot( slot );
}

ItemInformation::~ItemInformation( void )
{
	// Item information destroyed.
    size_t i;
    size_t length = attributes_.get_length();
    for (i = 0; i < length; ++i) {
        delete attributes_.get( i );
    }
    attributes_.clear();
}

const JUTIL::String* ItemInformation::get_name( void ) const
{
	return name_;
}

const JUI::Texture* ItemInformation::get_texture( void ) const
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
	attributes_.push( attribute );
}

size_t ItemInformation::get_attribute_count() const
{
	return attributes_.get_length();
}

const Attribute* ItemInformation::get_attribute( size_t index ) const
{
	return attributes_[index];
}

void ItemInformation::set_name( JUTIL::String* name )
{
	name_ = name;
}

void ItemInformation::set_texture( const JUI::Texture *texture )
{
	texture_ = texture;
}

void ItemInformation::set_class_flags( uint32 classFlags )
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

void ItemInformation::set_slot( EItemSlot slot )
{
	slot_ = slot;
}