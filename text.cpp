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

void Text::SetText( const string& text )
{
	// Set renderable string and pack.
	str_ = new RenderableCString( text.c_str(), text.length() );
	Pack();
}

void Text::SetText( const wchar_t* text, size_t length )
{
	// Set renderable wide string and pack.
	str_ = new RenderableWideString( text, length );
	Pack();
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

void Text::SetAlpha( int alpha )
{
	Component::SetAlpha( alpha );
	colour_.a = GetAlpha();
}

void Text::Draw( Graphics2D* graphics )
{
	glPushMatrix();
	glTranslatef( GetX(), GetY(), 0.0f );
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

void Text::Pack( void )
{
	if (str_ != nullptr) {
		// Pack on renderable string size.
		RECT size;
		font_->prepare_draw( &size, str_, list_ );
		SetSize( size.right, size.bottom );

		// Remove temporary renderable string.
		delete str_;
		str_ = nullptr;
	}
	else {
		SetSize( 0, 0 );
	}
}