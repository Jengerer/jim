#ifndef ITEM_DEFINITION_HPP
#define ITEM_DEFINITION_HPP

#include "item_shared.hpp"
#include "attribute.hpp"
#include "steam/SteamTypes.h"

#include <string/string.hpp>
#include <containers/vector.hpp>
#include <jui/gfx/texture.hpp>

/*
 * Item definition storage class.
 */
class ItemDefinition
{

public:

	ItemDefinition(
		JUTIL::String* name,
		const JUI::Texture *texture,
		uint32 class_flags,
		EItemSlot slot );
	~ItemDefinition( void );

	// Item information attributes.
	const JUTIL::String* get_name( void ) const;
	uint32 get_class_flags( void ) const;
	uint8 get_class_count( void ) const;
	EItemSlot get_slot( void ) const;
	uint32 get_tool_type( void ) const;
	void set_tool_type( uint32 tool_type );

	// Attribute functions.
	bool add_attribute( Attribute* attribute );
	size_t get_attribute_count( void ) const;
	const Attribute* get_attribute( size_t index ) const;
	const Attribute* find_attribute( const JUTIL::String* name ) const;

    // Get the texture for this item.
	const JUI::Texture* get_texture( void ) const;

private:

	// Private attribute setters.
	void set_name( JUTIL::String* name );
	void set_class_flags( uint32 class_flags );
	void set_slot( EItemSlot slot );

private:

    // Private item information attributes.
	JUTIL::String* name_;
	const JUI::Texture* texture_;
	uint32 class_flags_;
	uint8 class_count_;
	EItemSlot slot_;
	uint32 tool_type_;
	JUTIL::Vector<Attribute*> attributes_;

};

#endif // ITEM_DEFINITION_HPP