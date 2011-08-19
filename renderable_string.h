#ifndef IRENDERABLE_STRING_H
#define IRENDERABLE_STRING_H

#include <ft2build.h>
#include FT_FREETYPE_H

class RenderableString
{

public:

	RenderableString( size_t length );

	virtual FT_ULong char_code_at( size_t index ) const = 0;
	size_t length() const;

private:

	size_t length_;

};

#endif // IRENDERABLE_STRING_H