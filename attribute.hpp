#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include "attribute_definition.hpp"

// Union for attribute values, which can be either float or int.
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

	// Setting resolved attribute definition.
	void set_definition( const AttributeDefinition* definition );

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
	const AttributeDefinition* definition_;

	// String description for attribute.
    JUTIL::DynamicString description_;

};

#endif // ATTRIBUTE_H