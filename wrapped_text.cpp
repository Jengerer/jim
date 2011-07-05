#include "wrapped_text.h"

WrappedText::WrappedText( Font *font, int textWidth ) : Text( font )
{
	SetTextFormatting( 0 );
	SetTextWidth( textWidth );
}

WrappedText::~WrappedText( void )
{
	// Wrapped text destroyed.
}

void WrappedText::Draw( DirectX *directX )
{
	float x = GetX();
	float y = GetY();
	RECT textRect = {
		x, y,
		x + GetWidth(), y + GetHeight()
	};

	font_->drawText( GetText(), &textRect, GetTextFormatting(), ((GetAlpha() & 0xff) << 24) | (GetColour() & 0xFFFFFF) );
}

void WrappedText::Pack( void )
{	
	RECT resultSize = { 0, 0, GetTextWidth(), 0 };

	// Now calculate bottom.
	GetFont()->wrapText( text_, &resultSize, 0 );
	SetSize( GetTextWidth(), resultSize.bottom - resultSize.top );
}

int WrappedText::GetTextWidth( void ) const
{
	return textWidth_;
}

void WrappedText::SetTextWidth( int textWidth )
{
	textWidth_ = textWidth;
}

DWORD WrappedText::GetTextFormatting( void ) const
{
	return formatting_;
}

void WrappedText::SetTextFormatting( DWORD formatting )
{
	formatting_ = formatting;
}