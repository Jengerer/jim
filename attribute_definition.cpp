#include "attribute_definition.hpp"

/*
 * Attribute information constructor by attribute parameters.
 */
AttributeDefinition::AttributeDefinition( JUTIL::String* name,
    JUTIL::String* attribute_class,
	unsigned int index,
	float min_value,
	float max_value,
	bool hidden,
	bool is_integer )
{
	// Set main attributes.
	name_ = name;
	index_ = index;
	attribute_class_ = attribute_class;
	min_value_ = min_value;
	max_value_ = max_value;
	hidden_ = hidden;
	is_integer_ = is_integer;

    // No description by default.
    set_description( nullptr, nullptr );
}

/*
 * Attribute information destructor.
 */
AttributeDefinition::~AttributeDefinition( void )
{
    JUTIL::BaseAllocator::destroy( name_ );
    JUTIL::BaseAllocator::destroy( attribute_class_ );
    JUTIL::BaseAllocator::safe_destroy( &description_ );
    JUTIL::BaseAllocator::safe_destroy( &format_ );
}

/*
 * Get attribute name.
 */
const JUTIL::String* AttributeDefinition::get_name( void ) const
{
	return name_;
}

/*
 * Get attribute index.
 */
unsigned int AttributeDefinition::get_index( void ) const
{
	return index_;
}

/*
 * Get name of class for this attribute.
 */
const JUTIL::String* AttributeDefinition::get_attribute_class( void ) const
{
	return attribute_class_;
}

/*
 * Get minimum value for this attribute.
 */
float AttributeDefinition::get_min_value( void ) const
{
	return min_value_;
}

/*
 * Get maximum value for this attribute.
 */
float AttributeDefinition::get_max_value( void ) const
{
	return max_value_;
}

/*
 * Return whether this attribute should be hidden from the user.
 */
bool AttributeDefinition::is_hidden( void ) const
{
	return hidden_;
}

/*
 * Return whether this attribute value is an integer.
 */
bool AttributeDefinition::is_integer( void ) const
{
	return is_integer_;
}

/*
 * Return whether this attribute has a description string.
 */
bool AttributeDefinition::has_description( void ) const
{
	return description_ != nullptr;
}

/*
 * Set description string for this attribute.
 */
void AttributeDefinition::set_description( JUTIL::String* description, JUTIL::String* format )
{
    description_ = description;
    format_ = format;
}

const JUTIL::String* AttributeDefinition::get_description( void ) const
{
	return description_;
}

const JUTIL::String* AttributeDefinition::get_format( void ) const
{
	return format_;
}