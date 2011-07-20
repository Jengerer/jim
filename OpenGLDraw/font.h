#ifndef FONT_H
#define FONT_H

#include "exception.h"
#include "opengl_shared.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

class Font
{

public:

	Font( FT_Face face );

	void GenerateDrawList();

private:

	FT_Face face_;
	GLuint baseList_;
	GLuint textures_;
	

};

#endif // FONT_H