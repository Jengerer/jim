#include "image.h"

const Colour WHITE_TINT = { 255, 255, 255 };

Image::Image( Texture* texture, float localX, float localY ): Component( localX, localY )
{
	SetTexture( texture );
	SetTint( WHITE_TINT );
}

void Image::Draw( Graphics2D* graphics )
{
	// Draw texture.
	if (texture_ != nullptr) {
		graphics->set_colour( tint_, GetAlpha() );
		graphics->draw_texture(
			texture_,
			GetX(), GetY(),
			GetWidth(), GetHeight() );
	}
}

void Image::SetTint( const Colour& tint )
{
	tint_ = tint;
}

const Colour& Image::GetTint( void ) const
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