#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include "attribute_information.hpp"

class Attribute
{

public:

	Attribute( const AttributeInformation* attribute_info, float value );
	Attribute( const AttributeInformation* attribute_info, uint32 value );

	const AttributeInformation* get_attribute_info() const;
	std::string get_description_string() const;

	// Attribute information functions.
	unsigned int get_index() const;
	const std::string& get_name() const;

	// Value getters.
	float	get_float_value() const;
	uint32	get_uint32_value() const;
	
	// Information related functions.
	bool has_description() const;
	bool is_hidden() const;

private:

	void set_value( float value );
	void set_value( uint32 value );

private:

	float float_value_;
	uint32 uint32_value_;
	const AttributeInformation* attribute_info_;

};

#endif // ATTRIBUTE_H