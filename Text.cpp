#include "Text.h"

#define WHITE D3DCOLOR_XRGB( 255, 255, 255 )

Text::Text( Font *font )
{
	SetFont( font );
	SetColour( WHITE );
}

Text::~Text( void )
{
	// Text destroyed.
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

void Text::SetColour( D3DCOLOR colour )
{
	colour_ = colour;
}

D3DCOLOR Text::GetColour( void ) const
{
	return colour_;
}

void Text::OnDraw( DirectX *directX )
{
	float globalX = GetGlobalX();
	float globalY = GetGlobalY();
	RECT textRect = {
		globalX, globalY,
		globalX + GetWidth(),
		globalY + GetHeight() 
	};

	font_->drawText( text_, &textRect, 0, ((GetAlpha() & 0xff) << 24) | (GetColour() & 0xFFFFFF) );
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
	RECT resultSize;
	ZeroMemory( &resultSize, sizeof( RECT ) );
	font_->getTextRect( text_, &resultSize, 0 );
	SetSize( resultSize.right - resultSize.left, resultSize.bottom - resultSize.top );
}