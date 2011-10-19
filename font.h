#ifndef FONT_H
#define FONT_H

#include <stdexcept>
#include "renderable_string.h"
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
	void create_display_lists();

	// Character and line drawing.
	void draw_char( FT_ULong c ) const;
	void new_line() const;

	// Measurement functions.
	FT_Pos get_char_width( FT_ULong c ) const;
	FT_Pos get_string_width( const RenderableString* text, size_t start, size_t end ) const;

	// Regular char std::string drawing.
	void draw( const RenderableString* text, size_t start, size_t end ) const;
	void draw_aligned( const RenderableString* text, size_t start, size_t end, float width, TextHorizontalAlignType align_type ) const;
	void prepare_draw( RECT* rect, const RenderableString* text, GLuint list ) const;
	void prepare_wrap_draw( RECT* rect, const RenderableString* text, GLuint list, TextHorizontalAlignType align_type ) const;

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