#include "attribute.hpp"
#include <sstream>

Attribute::Attribute( const AttributeInformation* attribute_info, float value )
{
	attribute_info_ = attribute_info;
	set_value( value );
}

Attribute::Attribute( const AttributeInformation* attribute_info, uint32 value )
{
	attribute_info_ = attribute_info;
	set_value( value );
}

const AttributeInformation* Attribute::get_attribute_info( void ) const
{
	return attribute_info_;
}

/*
 * Generate description string if any.
 * Returns true on success, false otherwise.
 */
bool Attribute::generate_description( void )
{
    // std::string to replace.
	const JUTIL::ConstantString STRING_TOKEN = "%s1";

	// Get the description string.
    const JUTIL::String* description = attribute_info_->get_description();
	size_t start = description->find( &STRING_TOKEN );
    if (start != JUTIL::String::INVALID_INDEX) {
		// Write text up to token.
        if (!description_.copy( description->get_string(), start )) {
            return false;
        }

        // Write value instead of token.
		if (attribute_info_->is_integer()) {
            if (!description_.write( "%u", get_uint32_value() )) {
                return false;
            }
		}
		else {
            if (!description_.write( "%f", get_float_value() )) {
                return false;
            }
		}

        // Write the rest.
        size_t offset = start + STRING_TOKEN.get_length();
        if (!description_.write( "%s", description->get_string() + offset )) {
            return false;
        }
	}

	return true;
}

/*
 * Return constant handle to description string.
 */
const JUTIL::String* Attribute::get_description( void ) const
{
	return &description_;
}

/*
 * Get attribute index.
 */
unsigned int Attribute::get_index( void ) const
{
	return get_attribute_info()->get_index();
}

/*
 * Get constant handle to attribute name.
 */
const JUTIL::String* Attribute::get_name( void ) const
{
	return get_attribute_info()->get_name();
}

/*
 * Get value of attribute as a floating point.
 */
float Attribute::get_float_value( void ) const
{
    return value_.as_float_;
}

/*
 * Get value of attribute as an unsigned integer.
 */
uint32 Attribute::get_uint32_value( void ) const
{
    return value_.as_uint32_;
}

/*
 * Return whether this attribute has a text description.
 */
bool Attribute::has_description( void ) const
{
	return attribute_info_->has_description();
}

/*
 * Return whether this attribute should be hidden from the user.
 */
bool Attribute::is_hidden( void ) const
{
	return attribute_info_->is_hidden();
}

/*
 * Set the floating point value of this attribute.
 */
void Attribute::set_value( float value )
{
    value_.as_float_ = value;
}

/*
 * Set the unsigned integer value of this attribute.
 */
void Attribute::set_value( uint32 value )
{
    value_.as_uint32_ = value;
}