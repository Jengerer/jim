#include "image.h"

const D3DCOLOR WHITE_TINT = D3DCOLOR_XRGB( 255, 255, 255 );

Image::Image( Texture* texture, float localX, float localY ): Component( localX, localY )
{
	SetTexture( texture );
	SetTint( WHITE_TINT );
}

void Image::Draw( DirectX *directX )
{
	// Draw texture.
	if (texture_ != nullptr) {
		directX->DrawTexture(
			texture_,
			GetX(), GetY(),
			GetWidth(), GetHeight(), 
			(GetTint() & 0xFFFFFF) | ((GetAlpha() & 0xFF) << 24 ) );
	}
}

void Image::SetTint( D3DCOLOR tint )
{
	tint_ = tint;
}

D3DCOLOR Image::GetTint( void ) const
{
	return tint_;
}

void Image::SetTexture( Texture *texture )
{
	texture_ = texture;
}

Texture* Image::GetTexture( void ) const
{
	return texture_;
}