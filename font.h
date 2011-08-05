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

	void draw( const string& text );
	void draw_aligned( const string& text, float width, TextHorizontalAlignType align_type );
	void draw_aligned( const string& text, float width, float text_width, TextHorizontalAlignType align_type );
	void measure( RECT* rect, const string& text );
	void prepare_draw( float x, float y, const string& text, GLuint list );
	void prepare_wrap_draw( RECT* rect, const string& text, GLuint list );
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