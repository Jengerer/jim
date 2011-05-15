#include "WrappedText.h"

WrappedText::WrappedText( Font *font, int textWidth ) : Text( font )
{
	SetTextFormatting( 0 );
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

	font_->drawText( GetText(), &textRect, GetTextFormatting(), ((GetAlpha() & 0xff) << 24) | (GetColour() & 0xFFFFFF) );
}

void WrappedText::Pack( void )
{	
	RECT resultSize;
	resultSize.left		= GetX();
	resultSize.top		= GetY();
	resultSize.right	= resultSize.left + GetTextWidth();

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