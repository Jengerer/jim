#ifndef ITEM_INFORMATION_H
#define ITEM_INFORMATION_H

#include <string>
#include <vector>

#include "item_shared.h"
#include "attribute.h"
#include "texture.h"

#include "steam/SteamTypes.h"

class ItemInformation
{

public:

	ItemInformation(
		const std::string& name,
		Texture *texture,
		uint32 classFlags,
		EItemSlot slot );
	~ItemInformation( void );

	const std::string&	GetName( void ) const;
	const Texture*		GetTexture( void ) const;
	uint32			GetClassFlags( void ) const;
	uint8			GetClassCount( void ) const;
	EItemSlot		GetSlot( void ) const;

	void			AddAttribute( Attribute* attribute );
	size_t			GetAttributeCount() const;
	const Attribute* GetAttribute( size_t index ) const;

private:

	void			SetName( const std::string& name );
	void			SetTexture( const Texture *texture );
	void			SetClassFlags( uint32 classFlags );
	void			SetSlot( EItemSlot slot );

private:

	std::string				name_;
	const Texture*			texture_;
	uint32					classFlags_;
	uint8					classCount_;
	EItemSlot				slot_;
	std::vector<Attribute*> attributes_;

};

#endif // ITEM_INFORMATION_H