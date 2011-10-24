#include "renderable_cstring.h"
#include "renderable_wide_string.h"
#include "text.h"

Text::Text( Font *font )
{
	SetFont( font );
	SetColour( COLOUR_WHITE );
	list_ = glGenLists( 1 );
	str_ = nullptr;
}

Text::~Text( void )
{
	glDeleteLists( list_, 1 );
}

void Text::SetText( const std::string& text )
{
	// Set renderable std::string and pack.
	str_ = new RenderableCString( text.c_str(), text.length() );
	pack();
}

void Text::SetText( const wchar_t* text, size_t length )
{
	// Set renderable wide std::string and pack.
	str_ = new RenderableWideString( text, length );
	pack();
}

void Text::SetColour( const Colour& colour )
{
	colour_.r = colour.r;
	colour_.g = colour.g;
	colour_.b = colour.b;
}

const Colour& Text::GetColour( void ) const
{
	return colour_;
}

void Text::set_alpha( int alpha )
{
	Component::set_alpha( alpha );
	colour_.a = GetAlpha();
}

void Text::draw( Graphics2D* graphics )
{
	glPushMatrix();
	glTranslatef( get_x(), get_y(), 0.0f );
	graphics->set_colour( colour_ );
	glCallList( list_ );
	glPopMatrix();
}

Font* Text::GetFont( void ) const
{
	return font_;
}

void Text::SetFont( Font *font )
{
	font_ = font;
}

void Text::pack( void )
{
	if (str_ != nullptr) {
		// Pack on renderable std::string size.
		RECT size;
		font_->prepare_draw( &size, str_, list_ );
		set_size( size.right, size.bottom );

		// Remove temporary renderable string.
		delete str_;
		str_ = nullptr;
	}
	else {
		set_size( 0, 0 );
	}
}