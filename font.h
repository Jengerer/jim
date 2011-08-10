#ifndef FONT_H
#define FONT_H

#include "exception.h"
#include "opengl_shared.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

enum TextHorizontalAlignType {
	TEXT_ALIGN_LEFT,
	TEXT_ALIGN_CENTER,
	TEXT_ALIGN_RIGHT
};

class Font
{

public:

	Font( FT_Face face );
	~Font();

	void generate_glyphs();
	void create_display_list( unsigned char ch );

	void draw( const char* text, size_t length );
	void draw_aligned( const char* text, size_t length, float width, TextHorizontalAlignType align_type );
	void draw_aligned( const char* text, size_t length, float width, float text_width, TextHorizontalAlignType align_type );
	void measure( RECT* rect, const char* text, size_t length );
	void prepare_draw( const char* text, size_t length, GLuint list );
	void prepare_wrap_draw( RECT* rect, const char* text, size_t length, GLuint list, TextHorizontalAlignType align_type );
	void new_line();

	// Getting font attributes.
	GLsizei get_line_height() const;
	GLsizei get_baseline_spacing() const;

private:

	FT_Face		face_;
	GLuint*		textures_;
	GLuint		list_;
	FT_Pos*		advances_;
	
};

#endif // FONT_H