#include "Image.h"

Image::Image( float x, float y, Texture *texture ): Component( x, y )
{
	SetTexture( texture );
	SetSize( 0, 0 );
}

Image::~Image( void )
{
	// Image destroyed.
}

void Image::OnDraw( DirectX *directX )
{
	// Draw texture.
	if ( GetTexture() != nullptr ) {
		directX->DrawTexture( 
			GetTexture(), 
			GetGlobalX(), GetGlobalY(), 
			GetWidth(), GetHeight(), 
			D3DCOLOR_RGBA( 255, 255, 255, GetAlpha() ) );
	}
}

void Image::SetTexture( Texture *texture )
{
	texture_ = texture;
}

Texture* Image::GetTexture( void ) const
{
	return texture_;
}