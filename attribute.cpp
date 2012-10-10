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
	if (start != std::string::npos) {
		// Convert value to string.
		std::stringstream value_stream;
		if (attribute_info_->is_integer()) {
			value_stream << get_uint32_value();
		}
		else {
			value_stream << get_float_value();
		}
		const std::string& value_string = value_stream.str();

		// Replace token with value.
		desc_string.replace( start, strlen(STRING_TOKEN), value_string );
	}

	return desc_string;
}

const JUTIL::String* Attribute::get_description( void ) const
{
	return &description_;
}

unsigned int Attribute::get_index() const
{
	return get_attribute_info()->get_index();
}

const JUTIL::String* Attribute::get_name() const
{
	return get_attribute_info()->get_name();
}

float Attribute::get_float_value( void ) const
{
    return value_.as_float_;
}

uint32 Attribute::get_uint32_value( void ) const
{
    return value_.as_uint32_;
}

bool Attribute::has_description( void ) const
{
	return attribute_info_->has_description();
}

bool Attribute::is_hidden() const
{
	return attribute_info_->is_hidden();
}

void Attribute::set_value( float value )
{
    value_.as_float_ = value;
}

void Attribute::set_value( uint32 value )
{
    value_.as_uint32_ = value;
}