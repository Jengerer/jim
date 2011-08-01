#include "attribute.h"

#include <sstream>

Attribute::Attribute( AttributeInformation* attribute_info, float value )
{
	attribute_info_ = attribute_info;
	set_value( value );
}

Attribute::Attribute( AttributeInformation* attribute_info, uint32 value )
{
	attribute_info_ = attribute_info;
	set_value( value );
}

const AttributeInformation* Attribute::get_attribute_info() const
{
	return attribute_info_;
}

string Attribute::get_description_string() const
{
	// Get the description string.
	const string& desc_string = attribute_info_->get_desc_string();

	stringstream attribute_description;
	size_t index = desc_string.find( "%s1" );
	if (index != string::npos) {

	}

	return desc_string;
}

float Attribute::get_float_value() const
{
	return float_value_;
}

uint32 Attribute::get_uint32_value() const
{
	return uint32_value_;
}

void Attribute::set_value( float value )
{
	float_value_ = value;
}

void Attribute::set_value( uint32 value )
{
	uint32_value_ = value;
}