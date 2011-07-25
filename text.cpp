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
	colour_ = colour;
}

const Colour& Text::GetColour( void ) const
{
	return colour_;
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
	font_->measure( &size, text_ );
	font_->prepare_draw( GetX(), GetY(), text_, list_ );
	SetSize( size.right - size.left, size.bottom - size.top );
}