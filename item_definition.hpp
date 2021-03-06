#ifndef ITEM_DEFINITION_HPP
#define ITEM_DEFINITION_HPP

#include "item_shared.hpp"
#include "attribute.hpp"
#include "steam/SteamTypes.h"

#include <string/string.hpp>
#include <containers/vector.hpp>
#include <jui/gfx/texture.hpp>
#include <jui/gfx/graphics_2d.hpp>

/*
 * Item definition storage class.
 */
class ItemDefinition
{

public:

	ItemDefinition(
		JUTIL::String* name,
		JUTIL::DynamicString* image,
		uint32 class_flags );
	~ItemDefinition( void );

	// Item information attributes.
	const JUTIL::String* get_name( void ) const;
	uint32 get_class_flags( void ) const;
	uint8 get_class_count( void ) const;
	EItemToolType get_tool_type( void ) const;
	void set_tool_type( EItemToolType tool_type );

	// Attribute functions.
	bool add_attribute( Attribute* attribute );
	size_t get_attribute_count( void ) const;
	const Attribute* get_attribute( size_t index ) const;
	const Attribute* find_attribute( const JUTIL::String* name ) const;

    // Get the texture for this item.
	void load_texture( JUI::Graphics2D* graphics, const JUI::Texture* fallback );
	const JUI::Texture* get_texture( void ) const;

private:

	// Private attribute setters.
	void set_name( JUTIL::String* name );
	void set_class_flags( uint32 class_flags );

private:

    // Private item information attributes.
	JUTIL::String* name_;
	const JUI::Texture* texture_;
	JUTIL::DynamicString image_;
	uint32 class_flags_;
	uint8 class_count_;
	EItemToolType tool_type_;
	JUTIL::Vector<Attribute*> attributes_;

};

#endif // ITEM_DEFINITION_HPP