#ifndef ATTRIBUTE_DEFINITION
#define ATTRIBUTE_DEFINITION

#include "item_shared.hpp"
#include <string/string.hpp>

enum EffectType
{
	EFFECT_POSITIVE,
	EFFECT_NEGATIVE,
	EFFECT_NEUTRAL
};

class AttributeDefinition
{

public:

	AttributeDefinition( JUTIL::String* name,
		JUTIL::String* attribute_class,
        unsigned int index,
		float min_value,
		float max_value,
		EffectType effect_type,
		bool hidden,
		bool is_integer );
    virtual ~AttributeDefinition();

	// Accessing main attributes.
	const JUTIL::String* get_name( void ) const;
	unsigned int get_index( void ) const;
	const JUTIL::String* get_attribute_class( void ) const;
	float				get_min_value( void ) const;
	float				get_max_value( void ) const;
	EffectType			get_effect_type( void ) const;
	bool				is_hidden( void ) const;
	bool				is_integer( void ) const;

	// Optional attributes.
	bool has_description( void ) const;
	void set_description( JUTIL::String* description, JUTIL::String* format );
	const JUTIL::String* get_description( void ) const;
	const JUTIL::String* get_format( void ) const;

private:

	JUTIL::String* name_;
    JUTIL::String* attribute_class_;
	unsigned int index_;
	float min_value_;
	float max_value_;
	EffectType effect_type_;
	bool hidden_;
	bool is_integer_;

	// Optional members.
	JUTIL::String* description_;
	JUTIL::String* format_;

};

#endif // ATTRIBUTE_DEFINITION