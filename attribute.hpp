#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include "attribute_information.hpp"

union AttributeValue
{
	uint32 as_uint32;
	float as_float;
};

class Attribute
{

public:

	// Constructors by index or information.
	Attribute( uint32 index, AttributeValue value );
	Attribute( const AttributeInformation* attribute_info, AttributeValue value );

	// Attribute information management.
	void set_attribute_info( const AttributeInformation* info );
	const AttributeInformation* get_attribute_info( void ) const;

    // Description handling.
    bool generate_description( void );
    bool has_description( void ) const;
	const JUTIL::String* get_description( void ) const;

	// Attribute information functions.
	unsigned int get_index( void ) const;
	const JUTIL::String* get_name( void ) const;

	// Value function.
	void set_value( AttributeValue value );
	AttributeValue get_value( void ) const;
	
	// Information related functions.
	bool is_hidden( void ) const;

private:

    // Type can be either float or uint32.
    AttributeValue value_;

	// Attribute index and information structure.
	uint32 index_;
	const AttributeInformation* attribute_info_;

	// String description for attribute.
    JUTIL::DynamicString description_;

};

#endif // ATTRIBUTE_H