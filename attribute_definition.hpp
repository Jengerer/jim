#ifndef ATTRIBUTE_DEFINITION
#define ATTRIBUTE_DEFINITION

#include "item_shared.hpp"
#include <string/string.hpp>

class AttributeDefinition
{

public:

	AttributeDefinition( JUTIL::String* name,
        unsigned int index,
		bool hidden,
		bool is_integer );
    virtual ~AttributeDefinition();

	// Accessing main attributes.
	const JUTIL::String* get_name( void ) const;
	unsigned int get_index( void ) const;
	bool is_hidden( void ) const;
	bool is_integer( void ) const;

	// Optional attributes.
	bool has_description( void ) const;
	void set_description( JUTIL::String* description, JUTIL::String* format );
	const JUTIL::String* get_description( void ) const;
	const JUTIL::String* get_format( void ) const;

private:

	JUTIL::String* name_;
	unsigned int index_;
	bool hidden_;
	bool is_integer_;

	// Optional members.
	JUTIL::String* description_;
	JUTIL::String* format_;

};

#endif // ATTRIBUTE_DEFINITION