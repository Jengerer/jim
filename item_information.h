#ifndef ITEM_INFORMATION_H
#define ITEM_INFORMATION_H

#include <string>
#include <vector>

#include <jui/texture.h>

#include "item_shared.h"
#include "attribute.h"

#include "steam/SteamTypes.h"

/*
 * Item definition storage class.
 */
class ItemInformation
{

public:

	ItemInformation(
		const std::string& name,
		Texture *texture,
		uint32 classFlags,
		EItemSlot slot );
	~ItemInformation( void );

	// Item information attributes.
	const std::string&	get_name( void ) const;
	const Texture*		get_texture( void ) const;
	uint32				get_class_flags( void ) const;
	uint8				get_class_count( void ) const;
	EItemSlot			get_slot( void ) const;

	// Attribute functions.
	void				add_attribute( Attribute* attribute );
	size_t				get_attribute_count() const;
	const Attribute*	get_attribute( size_t index ) const;

private:

	// Private attribute setters.
	void				set_name( const std::string& name );
	void				set_texture( const Texture *texture );
	void				set_class_flags( uint32 classFlags );
	void				set_slot( EItemSlot slot );

private:

	std::string				name_;
	const Texture*			texture_;
	uint32					classFlags_;
	uint8					classCount_;
	EItemSlot				slot_;
	std::vector<Attribute*> attributes_;

};

#endif // ITEM_INFORMATION_H