#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include "attribute_definition.hpp"

/*
 * Generic attribute class; contains an index for attribute definition
 * and raw bytes of attribute value.
 */
class Attribute
{

public:

	// Constructors by index or information.
	Attribute( uint32 index );

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
	bool set_value( const char* buffer, unsigned int length );
	const char* get_value( void ) const;
	size_t get_value_length( void ) const;
	
	// Information related functions.
	bool is_hidden( void ) const;

private:

    // Value type is arbitrary, so stored raw; handled by caller by assuming/probing type.
	JUTIL::ArrayBuilder<char> value_;

	// Attribute index and information structure.
	uint32 index_;
	const AttributeDefinition* definition_;

	// String description for attribute.
    JUTIL::DynamicString description_;

};

#endif // ATTRIBUTE_H