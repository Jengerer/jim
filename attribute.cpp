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

const AttributeInformation* Attribute::get_attribute_info() const
{
	return attribute_info_;
}

std::string Attribute::get_description_string() const
{
	// std::string to replace.
	const char* STRING_TOKEN = "%s1";

	// Get the description string.
	std::string desc_string = attribute_info_->get_desc_string();
	size_t start = desc_string.find( STRING_TOKEN );
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

unsigned int Attribute::get_index() const
{
	return get_attribute_info()->get_index();
}

const std::string& Attribute::get_name() const
{
	return get_attribute_info()->get_name();
}

float Attribute::get_float_value() const
{
	return float_value_;
}

uint32 Attribute::get_uint32_value() const
{
	return uint32_value_;
}

bool Attribute::has_description() const
{
	return attribute_info_->has_description();
}

bool Attribute::is_hidden() const
{
	return attribute_info_->is_hidden();
}

void Attribute::set_value( float value )
{
	float_value_ = value;
}

void Attribute::set_value( uint32 value )
{
	uint32_value_ = value;
}