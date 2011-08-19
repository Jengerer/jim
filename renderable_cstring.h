#ifndef RENDERABLE_CSTRING_H
#define RENDERABLE_CSTRING_H

#include "renderable_string.h"

class RenderableCString : public RenderableString
{

public:

	RenderableCString( const char* str, size_t length );
	FT_ULong	char_code_at( size_t index ) const;

private:

	const char* str_;
	size_t length;

};

#endif // RENDERABLE_CSTRING_H