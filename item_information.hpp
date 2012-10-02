#ifndef ITEM_INFORMATION_H
#define ITEM_INFORMATION_H

#include "item_shared.hpp"
#include "attribute.hpp"
#include "steam/SteamTypes.h"

#include <string/string.hpp>
#include <containers/vector.hpp>
#include <jui/gfx/texture.hpp>

/*
 * Item definition storage class.
 */
class ItemInformation
{

public:

	ItemInformation(
		JUTIL::String* name,
		JUI::Texture *texture,
		uint32 classFlags,
		EItemSlot slot );
	~ItemInformation( void );

	// Item information attributes.
	const JUTIL::String* get_name( void ) const;
	const JUI::Texture* get_texture( void ) const;
	uint32 get_class_flags( void ) const;
	uint8 get_class_count( void ) const;
	EItemSlot get_slot( void ) const;

	// Attribute functions.
	void				add_attribute( Attribute* attribute );
	size_t				get_attribute_count() const;
	const Attribute*	get_attribute( size_t index ) const;

private:

	// Private attribute setters.
	void				set_name( JUTIL::String* name );
	void				set_texture( const JUI::Texture* texture );
	void				set_class_flags( uint32 classFlags );
	void				set_slot( EItemSlot slot );

private:

	JUTIL::String*              name_;
	const JUI::Texture*         texture_;
	uint32                      classFlags_;
	uint8                       classCount_;
	EItemSlot                   slot_;
	JUTIL::Vector<Attribute*>   attributes_;

};

#endif // ITEM_INFORMATION_H