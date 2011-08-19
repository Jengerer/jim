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
	RECT bounds = { 0, 0, GetTextWidth(), 0 };
	RenderableCString render_string( text_.c_str(), text_.length() );
	font_->prepare_wrap_draw( &bounds, &render_string, list_, TEXT_ALIGN_CENTER );
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