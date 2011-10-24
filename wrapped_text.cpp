#include "wrapped_text.h"
#include "renderable_cstring.h"

WrappedText::WrappedText( Font *font, int textWidth ) : Text( font )
{
	set_text_formatting( 0 );
	set_text_width( textWidth );
}

WrappedText::~WrappedText( void )
{
	// Wrapped text destroyed.
}

void WrappedText::pack( void )
{	
	if (str_ != nullptr) {
		// Wrap renderable string.
		RECT bounds = { 0, 0, get_text_width(), 0 };
		font_->prepare_wrap_draw( &bounds, str_, list_, TEXT_ALIGN_CENTER );
		set_size( get_text_width(), bounds.bottom - bounds.top );

		// Delete renderable string.
		delete str_;
		str_ = nullptr;
	}
	else {
		set_size( 0, 0 );
	}
}

int WrappedText::get_text_width( void ) const
{
	return textWidth_;
}

void WrappedText::set_text_width( int textWidth )
{
	textWidth_ = textWidth;
}

DWORD WrappedText::get_text_formatting( void ) const
{
	return formatting_;
}

void WrappedText::set_text_formatting( DWORD formatting )
{
	formatting_ = formatting;
}