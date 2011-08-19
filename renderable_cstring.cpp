#include "renderable_cstring.h"

RenderableCString::RenderableCString( const char* str, size_t length ) : RenderableString( length )
{
	str_ = str;
}

FT_ULong RenderableCString::char_code_at( size_t index ) const
{
	return static_cast<FT_ULong>(str_[index]);
}