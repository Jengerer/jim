#include "image.h"

Image::Image( Texture* texture, float localX, float localY ): Component( localX, localY )
{
	SetTexture( texture );
	SetTint( COLOUR_WHITE );
}

void Image::Draw( Graphics2D* graphics )
{
	// Draw texture.
	if (texture_ != nullptr) {
		graphics->set_colour( tint_ );
		graphics->draw_texture(
			texture_,
			GetX(), GetY(),
			GetWidth(), GetHeight() );
	}
}

void Image::SetAlpha( int alpha )
{
	Component::SetAlpha( alpha );
	tint_.a = GetAlpha();
}

void Image::SetTint( const Colour& tint )
{
	tint_.r = tint.r;
	tint_.g = tint.g;
	tint_.b = tint.b;
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