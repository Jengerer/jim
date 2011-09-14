#include "attribute_information.h"

AttributeInformation::AttributeInformation( const string& name,
	unsigned int index,
	const string& attribute_class,
	float min_value,
	float max_value,
	EffectType effect_type,
	bool hidden,
	bool is_integer )
{
	// Set main attributes.
	name_ = name;
	index_ = index;
	attribute_class_ = attribute_class;
	min_value_ = min_value;
	max_value_ = max_value;
	effect_type_ = effect_type;
	hidden_ = hidden;
	is_integer_ = is_integer;

	// No description yet.
	has_description_ = false;
}

const string& AttributeInformation::get_name() const
{
	return name_;
}

unsigned int AttributeInformation::get_index() const
{
	return index_;
}

const string& AttributeInformation::get_attribute_class() const
{
	return attribute_class_;
}

float AttributeInformation::get_min_value() const
{
	return min_value_;
}

float AttributeInformation::get_max_value() const
{
	return max_value_;
}

EffectType AttributeInformation::get_effect_type() const
{
	return effect_type_;
}

bool AttributeInformation::is_hidden() const
{
	return hidden_;
}

bool AttributeInformation::is_integer() const
{
	return is_integer_;
}

bool AttributeInformation::has_description() const
{
	return has_description_;
}

void AttributeInformation::set_description( const string& desc_string, const string& desc_format )
{
	desc_string_ = desc_string;
	desc_format_ = desc_format;
	has_description_ = true;
}

const string& AttributeInformation::get_desc_string() const
{
	return desc_string_;
}

const string& AttributeInformation::get_desc_format() const
{
	return desc_format_;
}