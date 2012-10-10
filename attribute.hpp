#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include "attribute_information.hpp"

class Attribute
{

public:

	Attribute( const AttributeInformation* attribute_info, float value );
	Attribute( const AttributeInformation* attribute_info, uint32 value );

	const AttributeInformation* get_attribute_info( void ) const;

    // Description handling.
    bool generate_description( void );
    bool has_description( void ) const;
	const JUTIL::String* get_description( void ) const;

	// Attribute information functions.
	unsigned int get_index( void ) const;
	const JUTIL::String* get_name( void ) const;

	// Value getters.
	float	get_float_value( void ) const;
	uint32	get_uint32_value( void ) const;
	
	// Information related functions.
	bool is_hidden( void ) const;

private:

	void set_value( float value );
	void set_value( uint32 value );

private:

    // Type can be either float or uint32.
    union {
	    float as_float_;
	    uint32 as_uint32_;
    } value_;

	const AttributeInformation* attribute_info_;
    JUTIL::DynamicString description_;

};

#endif // ATTRIBUTE_H