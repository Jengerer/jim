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

void Font::draw( const string& text )
{
	glListBase( list_ );
	glCallLists( text.length(), GL_UNSIGNED_BYTE, text.c_str() );
}

void Font::draw_aligned( const string& text, float width, float text_width, TextHorizontalAlignType align_type )
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
	draw( text );
	glPopMatrix();
}

void Font::draw_aligned( const string& text, float width, TextHorizontalAlignType align_type )
{
	// Measure the line.
	RECT text_rect;
	measure( &text_rect, text );
	float text_width = static_cast<float>(text_rect.right - text_rect.left);
	draw_aligned( text, width, text_width, align_type );
}

void Font::measure( RECT* rect, const string& text )
{
	// Measure width.
	FT_Pos right = 0L;
	for (auto i = text.begin(); i != text.end(); ++i) {
		right += advances_[*i];
	}

	// Set bounds.
	rect->left = 0L;
	rect->top = 0L;
	rect->bottom = face_->size->metrics.ascender >> 6;
	rect->right = right;
}

void Font::prepare_draw( float x, float y, const string& text, GLuint list )
{
	// Start the list on a new matrix.
	glNewList( list, GL_COMPILE );
	glPushMatrix();
	glTranslatef( x, y, 0.0f );
	
	// Keep making new lines.
	size_t start = 0;
	for (size_t i = 0; i < text.length(); ++i) {
		if (text[i] == '\n') {
			string line = text.substr( start, i - start );

			// Draw text and set new line.
			glPushMatrix();
			draw( line );
			glPopMatrix();
			new_line();
			start = i + 1;
		}
	}

	// Draw the last line.
	string line = text.substr( start );
	draw( line );

	// End the list.
	glPopMatrix();
	glEndList();
}

void Font::prepare_wrap_draw( RECT* bounds, const string& text, GLuint list )
{
	// Word wrapping variables.
	bool first = true;
	LONG width = bounds->right - bounds->left;
	unsigned int new_lines = 0;

	// Indexes.
	size_t prev = 0;
	size_t start = 0;

	// Start the list on a new matrix.
	glNewList( list, GL_COMPILE );
	glPushMatrix();
	glTranslatef( bounds->left, bounds->top, 0.0f );

	// Start wrapping.
	for (size_t i = 0; i < text.length(); i++) {
		char current_char = text[i];

		// Check whether this a newline character.
		bool is_newline = (current_char == '\n');

		// Check if we should test fitting.
		if (current_char == ' ' || is_newline) {
			const string& current = text.substr( start, i - start );

			RECT rect;
			measure( &rect, current );
			long text_width = rect.right - rect.left;

			// Check whether this line exceeds bounds.
			bool has_drawn = false;
			if (text_width > width) {
				if (first) {
					start = i + 1;
					draw_aligned( current, width, text_width, TEXT_ALIGN_CENTER );
					has_drawn = true;
				}
				else {
					const string& last = text.substr( start, prev - start );
					draw_aligned( last, width, TEXT_ALIGN_CENTER );
					start = prev + 1;
					has_drawn = true;
				}
			}
			else {
				first = false;
			}
		}
	}

	// Finish the last line.
	const string& final = text.substr( start );
	RECT rect = {0, 0, 0, 0};
	measure( &rect, final );
	long text_width = rect.right - rect.left;
	if (text_width > width && !first) {
		// Not the first word, draw last and then first.
		const string& last = text.substr( start, prev - start );
		draw_aligned( last, width, TEXT_ALIGN_CENTER );

		// Finish last.
		new_line();
		new_lines++;
		const string& end = text.substr( prev + 1 );
		draw_aligned( end, width, TEXT_ALIGN_CENTER );
	}
	else {
		draw_aligned( final, width, TEXT_ALIGN_CENTER );
	}

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