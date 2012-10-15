#include "item_information.hpp"

/*
 * Constructor for storing item information by item parameters.
 */
ItemInformation::ItemInformation(
	JUTIL::String* name,
	const JUI::Texture* texture,
	uint32 class_flags,
	EItemSlot slot )
{
	set_name( name );
	set_texture( texture );
	set_class_flags( class_flags );
	set_slot( slot );
}

/*
 * Item information destructor.
 */
ItemInformation::~ItemInformation( void )
{
	// Item information destroyed.
    size_t i;
    size_t length = attributes_.get_length();
    for (i = 0; i < length; ++i) {
        Attribute* attribute = attributes_.get( i );
        JUTIL::BaseAllocator::destroy( attribute );
    }
    attributes_.clear();

    // Destroy name.
    JUTIL::BaseAllocator::destroy( name_ );
}

/*
 * Get handle to item name string.
 */
const JUTIL::String* ItemInformation::get_name( void ) const
{
	return name_;
}

/*
 * Get texture for item icon.
 */
const JUI::Texture* ItemInformation::get_texture( void ) const
{
	return texture_;
}

/*
 * Get class flags for item type.
 */
uint32 ItemInformation::get_class_flags( void ) const
{
	return class_flags_;
}

/*
 * Get class count for item type.
 */
uint8 ItemInformation::get_class_count( void ) const
{
	return class_count_;
}

/*
 * Get equipment slot for item.
 */
EItemSlot ItemInformation::get_slot( void ) const
{
	return slot_;
}

/*
 * Add an attribute generic to this item type.
 */
bool ItemInformation::add_attribute( Attribute* attribute )
{
	return attributes_.push( attribute );
}

/*
 * Get the number of attributes generic to this item type.
 */
size_t ItemInformation::get_attribute_count( void ) const
{
	return attributes_.get_length();
}

/*
 * Get an attribute by index.
 */
const Attribute* ItemInformation::get_attribute( size_t index ) const
{
	return attributes_.get( index );
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
	class_flags_ = classFlags;

	// Now count the number of classes.
	class_count_ = 0;
	while (classFlags != 0) {
		if ((classFlags & 0x1) != 0) {
			class_count_++;
		}

		classFlags = classFlags >> 1;
	}
}

void ItemInformation::set_slot( EItemSlot slot )
{
	slot_ = slot;
}