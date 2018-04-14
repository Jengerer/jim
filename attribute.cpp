#include "attribute.hpp"
#include "jui/application/error_stack.hpp"

/*
 * Constructor by index and value.
 */
Attribute::Attribute( uint32 index ) :
	value_(),
	index_( index ),
	definition_( nullptr ),
	description_()
{
}

/*
 * Set definition from schema.
 */
void Attribute::set_definition( const AttributeDefinition* definition )
{
	definition_ = definition;
}

/*
 * Generate description string if any.
 * Returns true on success, false otherwise.
 */
bool Attribute::generate_description( void )
{
	// Check if anything to generate.
	if (!definition_->has_description()) {
		return true;
	}

    // String to replace.
	const JUTIL::ConstantString STRING_TOKEN = "%s1";

	// Get the description string.
    const JUTIL::String* description = definition_->get_description();
	size_t start = description->find( &STRING_TOKEN );
    if (start != JUTIL::String::INVALID_INDEX) {
		// Write text up to token.
        if (!description_.copy( description->get_string(), start )) {
            return false;
        }

        // Write value instead of token.
		const char* attribute_value = value_.get_array();
		if (definition_->is_integer()) {
			const uint32* int_value = reinterpret_cast<const uint32*>(attribute_value);
			if (!description_.write( "%u", *int_value )) {
                return false;
            }
		}
		else {
			const float* float_value = reinterpret_cast<const float*>(attribute_value);
			if (!description_.write( "%f", *float_value )) {
                return false;
            }
		}

        // Write the rest.
        size_t offset = start + STRING_TOKEN.get_length();
        if (!description_.write( "%s", description->get_string() + offset )) {
            return false;
        }
	}
	else {
		// No token, just copy the string.
		if (!description_.copy( description )) {
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
	return index_;
}

/*
 * Get constant handle to attribute name.
 */
const JUTIL::String* Attribute::get_name( void ) const
{
	return definition_->get_name();
}

/*
 * Set new value for attribute.
 */
bool Attribute::set_value( const char* buffer, unsigned int length )
{
	// Copy to local.
	if (!value_.set_size( length )) {
		return false;
	}
	memcpy( value_.get_array(), buffer, length );
	return true;
}

/*
 * Get value of attribute as an unsigned integer.
 */
const char* Attribute::get_value( void ) const
{
    return value_.get_array();
}

/*
 * Get the length of the value.
 */
size_t Attribute::get_value_length( void ) const
{
	return value_.get_size();
}

/*
 * Return whether this attribute has a text description.
 */
bool Attribute::has_description( void ) const
{
	return definition_->has_description();
}

/*
 * Return whether this attribute should be hidden from the user.
 */
bool Attribute::is_hidden( void ) const
{
	return definition_->is_hidden();
}