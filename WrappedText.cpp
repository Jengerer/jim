#include "WrappedText.h"

WrappedText::WrappedText( Font *font, int textWidth ) : Text( font )
{
	SetTextWidth( textWidth );
}

WrappedText::~WrappedText( void )
{
	// Wrapped text destroyed.
}

void WrappedText::OnDraw( DirectX *directX )
{
	RECT textRect = {
		GetX(),
		GetY(),
		GetX() + GetWidth(),
		GetY() + GetHeight()
	};

	font_->drawText( text_, &textRect, GetTextFormatting(), GetColour() );
}

void WrappedText::Pack( void )
{	
	RECT resultSize = { 0, 0, GetTextWidth(), 0 };
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