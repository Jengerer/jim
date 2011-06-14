#ifndef ITEM_INFORMATION_H
#define ITEM_INFORMATION_H

#include <string>

#include "item_shared.h"
#include "texture.h"

#include "steam/SteamTypes.h"

class CItemInformation
{

public:

	CItemInformation(
		const string& name,
		Texture *texture,
		uint32 classFlags,
		EItemSlot slot );
	~CItemInformation( void );

	const string&	GetName( void ) const;
	Texture*		GetTexture( void ) const;
	uint32			GetClassFlags( void ) const;
	uint8			GetClassCount( void ) const;
	EItemSlot		GetSlot( void ) const;

private:

	void			SetName( const string& name );
	void			SetTexture( Texture *texture );
	void			SetClassFlags( uint32 classFlags );
	void			SetSlot( EItemSlot slot );

private:

	string			name_;
	Texture*		texture_; // TODO: Can this be const?
	uint32			classFlags_;
	uint8			classCount_;
	EItemSlot		slot_;

};

#endif // ITEM_INFORMATION_H