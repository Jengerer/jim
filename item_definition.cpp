#include "item_definition.hpp"

/*
 * Constructor for storing item information by item parameters.
 */
ItemDefinition::ItemDefinition(
	JUTIL::String* name,
	const JUI::Texture* texture,
	uint32 class_flags,
	EItemSlot slot )
{
	set_name( name );
	add_texture( texture );
	set_class_flags( class_flags );
	set_slot( slot );
	set_tool_type( TOOL_NOT_A_TOOL );
}

/*
 * Item information destructor.
 */
ItemDefinition::~ItemDefinition( void )
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
const JUTIL::String* ItemDefinition::get_name( void ) const
{
	return name_;
}

/*
 * Get class flags for item type.
 */
uint32 ItemDefinition::get_class_flags( void ) const
{
	return class_flags_;
}

/*
 * Get class count for item type.
 */
uint8 ItemDefinition::get_class_count( void ) const
{
	return class_count_;
}

/*
 * Get equipment slot for item.
 */
EItemSlot ItemDefinition::get_slot( void ) const
{
	return slot_;
}

/*
 * Get item tool type.
 */
uint32 ItemDefinition::get_tool_type( void ) const
{
	return tool_type_;
}

/*
 * Add an attribute generic to this item type.
 */
bool ItemDefinition::add_attribute( Attribute* attribute )
{
	return attributes_.push( attribute );
}

/*
 * Get the number of attributes generic to this item type.
 */
size_t ItemDefinition::get_attribute_count( void ) const
{
	return attributes_.get_length();
}

/*
 * Get an attribute by index.
 */
const Attribute* ItemDefinition::get_attribute( size_t index ) const
{
	return attributes_.get( index );
}

/*
 * Checks for attribute by name.
 * Returns nullptr if not found.
 */
const Attribute* ItemDefinition::find_attribute( const JUTIL::String* name ) const
{
    // Find attribute with matching index.
    size_t i;
    size_t length = get_attribute_count();
    for (i = 0; i < length; ++i) {
        const Attribute* attribute = get_attribute( i );
        const JUTIL::String* attribute_name = attribute->get_name();
        if (name->is_equal( attribute_name )) {
			return attribute;
		}
	}

	// Not found, return nullptr.
	return nullptr;
}

/*
 * Add a texture generic to this item type.
 */
bool ItemDefinition::add_texture( const JUI::Texture* texture)
{
	return textures_.push( texture );
}

/*
 * Get the number of textures generic to this item type.
 */
size_t ItemDefinition::get_texture_count( void ) const
{
	return textures_.get_length();
}

/*
 * Get a texture by index.
 */
const JUI::Texture* ItemDefinition::get_texture( size_t index ) const
{
	return textures_.get( index );
}


void ItemDefinition::set_name( JUTIL::String* name )
{
	name_ = name;
}

void ItemDefinition::set_class_flags( uint32 class_flags )
{
	class_flags_ = class_flags;

	// Now count the number of classes.
	class_count_ = 0;
	while (class_flags != 0) {
		if ((class_flags & 0x1) != 0) {
			class_count_++;
		}

		class_flags = class_flags >> 1;
	}
}

void ItemDefinition::set_slot( EItemSlot slot )
{
	slot_ = slot;
}

void ItemDefinition::set_tool_type( uint32 tool_type )
{
	tool_type_ = tool_type;
}