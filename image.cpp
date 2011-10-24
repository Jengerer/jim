#include "image.h"

Image::Image( const Texture* texture, float localX, float localY ): Component( localX, localY )
{
	set_texture( texture );
	set_tint( COLOUR_WHITE );
}

void Image::draw( Graphics2D* graphics )
{
	// Draw texture.
	if (texture_ != nullptr) {
		graphics->set_colour( tint_ );
		graphics->draw_texture(
			texture_,
			get_x(), get_y(),
			get_width(), get_height() );
	}
}

void Image::set_alpha( int alpha )
{
	Component::set_alpha( alpha );
	tint_.a = GetAlpha();
}

void Image::set_tint( const Colour& tint )
{
	tint_.r = tint.r;
	tint_.g = tint.g;
	tint_.b = tint.b;
}

const Colour& Image::get_tint( void ) const
{
	return tint_;
}

void Image::set_texture( const Texture *texture )
{
	texture_ = texture;
}

const Texture* Image::get_texture( void ) const
{
	return texture_;
}