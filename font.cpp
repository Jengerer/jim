#include "font.h"

Font::Font( FT_Face face )
{
	face_ = face;
	FT_Reference_Face( face );

	// Null/zero lists and textures.
	list_ = 0;
	textures_ = nullptr;
	advances_ = nullptr;
}

Font::~Font()
{
	unsigned int glyph_count = face_->num_glyphs;

	// Close the face.
	FT_Done_Face( face_ );

	// Delete textures.
	if (textures_ != nullptr) {
		glDeleteTextures( glyph_count, textures_ );
		delete[] textures_;
		textures_ = nullptr;
	}

	// Delete list.
	if (list_ != 0) {
		glDeleteLists( list_, glyph_count );
	}
	
	// Delete advances.
	if (advances_ != nullptr) {
		delete[] advances_;
		advances_ = nullptr;
	}
}

void Font::generate_glyphs()
{
	unsigned int glyph_count = face_->num_glyphs;

	// Create texture and display list.
	textures_ = new GLuint[ glyph_count ];
	glGenTextures( glyph_count, textures_ );
	list_ = glGenLists( glyph_count );
	advances_ = new FT_Pos[ glyph_count ];

	// Generate display lists for all characters.
	for (unsigned char i = 0; i < 255; ++i) {
		create_display_list( i );
	}
}

void Font::create_display_list( unsigned char ch )
{
	FT_UInt index = FT_Get_Char_Index( face_, ch );

	// Load the glyph for the character.
	FT_Error error = FT_Load_Glyph( face_, index, FT_LOAD_DEFAULT );
	if (error != 0) {
		throw Exception( "Failed to load glyph." );
	}

	// Render the glyph.
	error = FT_Render_Glyph( face_->glyph, FT_RENDER_MODE_NORMAL );
	if (error != 0) {
		throw Exception( "Failed to render glyph." );
	}

	// Get the texture size.
	FT_Bitmap& bitmap = face_->glyph->bitmap;
	GLsizei width = next_power_of_2( bitmap.width );
	GLsizei height = next_power_of_2( bitmap.rows );

	// Create buffer for pixels.
	const unsigned int SRC_BPP = 1;
	const unsigned int DEST_BPP = 2;
	GLubyte* tex_buffer = new GLubyte[ DEST_BPP * width * height ]; 
	const GLsizei DEST_ROW_WIDTH = DEST_BPP * width;
	const GLsizei DEST_SRC_WIDTH = DEST_BPP * bitmap.width;
	const GLsizei ROW_REMAINDER = DEST_ROW_WIDTH - DEST_BPP * bitmap.width;

	// Fill in alpha and colour.
	for (GLsizei y = 0; y < bitmap.rows; ++y) {
		GLubyte* row_start = tex_buffer + y * DEST_ROW_WIDTH;
		for (GLsizei x = 0; x < bitmap.width; ++x) {
			unsigned int src_index = SRC_BPP * (x + bitmap.width * y);
			unsigned int dest_index = DEST_BPP * (x + y * width);
			tex_buffer[ dest_index ] = 0xff;
			tex_buffer[ dest_index + 1 ]  = bitmap.buffer[ src_index ];
		}

		// Zero the rest.
		memset( row_start + DEST_SRC_WIDTH, 0, ROW_REMAINDER);
	}

	// Set up texture params.
	glBindTexture( GL_TEXTURE_2D, textures_[ch] );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

	// Store character advance.
	advances_[ch] = face_->glyph->advance.x >> 6;

	// Create texture from the buffer.
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 
		width, height, 0,
		GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE,
		tex_buffer );
	delete[] tex_buffer;

	// Create display list for this character.
	glNewList( list_ + ch, GL_COMPILE );
	glBindTexture( GL_TEXTURE_2D, textures_[ch] );
	glPushMatrix();

	// Place the character properly.
	glTranslatef( face_->glyph->bitmap_left, 0.0f, 0.0f );
	glTranslatef( 0, (face_->size->metrics.ascender >> 6) - face_->glyph->bitmap_top, 0.0f );

	// Get texture coordinates (due to power-of-2 rule).
	float x = static_cast<float>(bitmap.width) / static_cast<float>(width);
	float y = static_cast<float>(bitmap.rows) / static_cast<float>(height);

	// Draw texture mapped quad.
	glBegin( GL_QUADS );
		glTexCoord2f( 0.0f, 0.0f );
		glVertex2f( 0.0f, 0.0f );

		glTexCoord2f( x, 0.0f );
		glVertex2f( bitmap.width, 0.0f );

		glTexCoord2f( x, y );
		glVertex2f( bitmap.width, bitmap.rows );

		glTexCoord2f( 0.0f, y );	
		glVertex2f( 0.0f, bitmap.rows );
	glEnd();
	glPopMatrix();

	// Move forward by the character's advance.
	glTranslatef( face_->glyph->advance.x >> 6, 0.0f, 0.0f );
	glBindTexture( GL_TEXTURE_2D, 0 );
	glEndList();
}

void Font::draw( const char* text, size_t length )
{
	glListBase( list_ );
	glCallLists( length, GL_UNSIGNED_BYTE, (unsigned char*)text );
}

void Font::draw_aligned( const char* text, size_t length, float width, float text_width, TextHorizontalAlignType align_type )
{
	// Translate to alignment.
	glPushMatrix();
	switch (align_type) {
	case TEXT_ALIGN_CENTER:
		glTranslatef( (width - text_width) / 2.0f, 0.0f, 0.0f );
		break;
	case TEXT_ALIGN_RIGHT:
		glTranslatef( (width - text_width), 0.0f, 0.0f );
		break;
	}

	// Draw text.
	draw( text, length );
	glPopMatrix();
}

void Font::draw_aligned( const char* text, size_t length, float width, TextHorizontalAlignType align_type )
{
	// Measure the line.
	RECT text_rect;
	measure( &text_rect, text, length );
	float text_width = static_cast<float>(text_rect.right - text_rect.left);
	draw_aligned( text, length, width, text_width, align_type );
}

void Font::measure( RECT* rect, const char* text, size_t length )
{
	// Measure width.
	FT_Pos right = 0L;
	for (size_t i = 0; i < length; ++i) {
		right += advances_[text[i]];
	}

	// Set bounds.
	rect->left = 0L;
	rect->top = 0L;
	rect->bottom = get_line_height();
	rect->right = right;
}

void Font::prepare_draw( const char* text, size_t length, GLuint list )
{
	// Start the list on a new matrix.
	glNewList( list, GL_COMPILE );
	glPushMatrix();
	
	// Keep making new lines.
	size_t start = 0;
	for (size_t i = 0; i < length; ++i) {
		if (text[i] == '\n') {
			const char* line = text + start;
			size_t line_length = i - start;

			// Draw text and set new line.
			glPushMatrix();
			draw( line, line_length );
			glPopMatrix();
			new_line();

			// Next line starts at next char.
			start = i + 1;
		}
	}

	// Draw the last line.
	const char* last_line = text + start;
	draw( text + start, length - start );

	// End the list.
	glPopMatrix();
	glEndList();
}

void Font::prepare_wrap_draw( RECT* bounds, const char* text, size_t length, GLuint list, TextHorizontalAlignType align_type )
{
	// Constant bounds.
	const long LINE_WIDTH = bounds->right - bounds->left;

	// Line break variables.
	size_t line_start = 0;
	size_t break_point = 0;
	long width_left = LINE_WIDTH;
	long width_since_break = 0;
	unsigned int new_lines = 0;

	// Begin list.
	glNewList( list, GL_COMPILE );

	// Set local temporary transformation.
	glPushMatrix();

	for (size_t i = 0; i < length; ++i) {
		char ch = text[i];
		bool draw_line = false;
		
		if (ch == '\n') {
			// It has fit so far, draw until here.
			break_point = i;
			draw_line = true;
			width_since_break = 0;
		}
		else {
			// Move forward by character width.
			long char_width = advances_[(unsigned char)ch];
			width_since_break += char_width;

			// Check if exceeding and if we have a break.
			bool exceeds_width = width_since_break > width_left;
			bool no_break = break_point == line_start;
			
			if (exceeds_width) {
				if (ch == ' ') {
					// Use space to break line.
					break_point = i;
					width_since_break = 0;
				}
				else if (no_break) {
					// Set break before this character.
					break_point = i - 1;
					width_since_break = char_width;
				}
				
				draw_line = 1;
			}
			else if (ch == '/' || ch == ' ' || ch == '-') {
				// Reset width since break.
				width_left -= width_since_break;
				width_since_break = 0;

				// Now set this as the break point.
				break_point = i;
			}
		}
		
		if (draw_line) {
			draw_aligned( text + line_start, break_point - line_start, LINE_WIDTH, align_type );
			new_line();
			new_lines++;

			// Reset start of line and remaining width.
			line_start = break_point = break_point + 1;
			width_left = LINE_WIDTH;
		}
	}

	// Draw last line, since it fits.
	draw_aligned( text + line_start, length - line_start, LINE_WIDTH, align_type );

	// Adjust rect bounds by this size.
	bounds->bottom = bounds->top + (new_lines * get_baseline_spacing()) + get_line_height();
	glPopMatrix();
	glEndList();
}

void Font::new_line()
{
	glTranslatef( 0.0f,
		get_baseline_spacing(),
		0.0f );
}

GLsizei Font::get_line_height() const
{
	return face_->size->metrics.ascender >> 6;
}

GLsizei Font::get_baseline_spacing() const
{
	return face_->size->metrics.height >> 6;
}