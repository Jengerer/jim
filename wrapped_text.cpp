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

void WrappedText::Pack( void )
{	
	RECT bounds = { 0, 0, GetTextWidth(), 0 };
	font_->prepare_wrap_draw( &bounds, text_, list_ );
	SetSize( GetTextWidth(), bounds.bottom - bounds.top );
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