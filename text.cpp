#include "renderable_cstring.h"
#include "text.h"

Text::Text( Font *font )
{
	SetFont( font );
	SetColour( COLOUR_WHITE );
	list_ = glGenLists( 1 );
}

Text::~Text( void )
{
	glDeleteLists( list_, 1 );
}

void Text::SetText( const string& text )
{
	text_ = text;
	Pack();
}

const string& Text::GetText( void ) const
{
	return text_;
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
	RECT size;
	RenderableCString render_string( text_.c_str(), text_.size() );
	font_->prepare_draw( &size, &render_string, list_ );
	SetSize( size.right, size.bottom );
}