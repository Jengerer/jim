#include "image.h"

Image::Image( Texture* texture, float localX, float localY ): Component( localX, localY )
{
	SetTexture( texture );
}

void Image::OnDraw( DirectX *directX )
{
	// Draw texture.
	if (texture_ != nullptr) {
		directX->DrawTexture(
			texture_,
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