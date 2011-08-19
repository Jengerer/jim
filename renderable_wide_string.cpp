#include "renderable_wide_string.h"

RenderableWideString::RenderableWideString( const wchar_t* wstr, size_t length ) : RenderableString( length )
{
	wstr_ = wstr;
}

FT_ULong RenderableWideString::char_code_at( size_t index ) const
{
	return static_cast<FT_ULong>(wstr_[index]);
}