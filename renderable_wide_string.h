#ifndef RENDERABLE_WIDE_STRING_H
#define RENDERABLE_WIDE_STRING_H

#include "renderable_string.h"

class RenderableWideString : public RenderableString
{

public:

	RenderableWideString( const wchar_t* wstr, size_t length );
	FT_ULong	char_code_at( size_t index ) const;

private:

	const wchar_t* wstr_;

};

#endif // RENDERABLE_WIDE_STRING_H