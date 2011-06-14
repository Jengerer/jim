#include "item_information.h"

CItemInformation::CItemInformation(
	const string& name,
	Texture *texture,
	uint32 classFlags,
	EItemSlot slot )
{
	SetName( name );
	SetTexture( texture );
	SetClassFlags( classFlags );
	SetSlot( slot );
}

CItemInformation::~CItemInformation( void )
{
	// Item information destroyed.
}

const string& CItemInformation::GetName( void ) const
{
	return name_;
}

Texture* CItemInformation::GetTexture( void ) const
{
	return texture_;
}

uint32 CItemInformation::GetClassFlags( void ) const
{
	return classFlags_;
}

uint8 CItemInformation::GetClassCount( void ) const
{
	return classCount_;
}

EItemSlot CItemInformation::GetSlot( void ) const
{
	return slot_;
}

void CItemInformation::SetName( const string& name )
{
	name_ = name;
}

void CItemInformation::SetTexture( Texture *texture )
{
	texture_ = texture;
}

void CItemInformation::SetClassFlags( uint32 classFlags )
{
	classFlags_ = classFlags;

	// Now count the number of classes.
	classCount_ = 0;
	while (classFlags != 0) {
		if ((classFlags & 0x1) != 0) {
			classCount_++;
		}

		classFlags = classFlags >> 1;
	}
}

void CItemInformation::SetSlot( EItemSlot slot )
{
	slot_ = slot;
}