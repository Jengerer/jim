#include "wrapped_text.h"
#include "renderable_cstring.h"

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
	if (str_ != nullptr) {
		// Wrap renderable string.
		RECT bounds = { 0, 0, GetTextWidth(), 0 };
		font_->prepare_wrap_draw( &bounds, str_, list_, TEXT_ALIGN_CENTER );
		SetSize( GetTextWidth(), bounds.bottom - bounds.top );

		// Delete renderable string.
		delete str_;
		str_ = nullptr;
	}
	else {
		SetSize( 0, 0 );
	}
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