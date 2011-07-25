#include "font.h"

GLuint rofl = 0;
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

	rofl = textures_[0];
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
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

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

	unsigned char breakch = 'a';
	if (ch == breakch) {
		int i = 1;
	}

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

void Font::draw( float x, float y, const string& text )
{
	glPushMatrix();
	glTranslatef( x, y, 0.0f );
	glListBase( list_ );
	glCallLists( text.length(), GL_UNSIGNED_BYTE, text.c_str() );
	glPopMatrix();
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
			draw( 0.0f, 0.0f, line );
			glTranslatef( 0.0f, face_->size->metrics.height >> 6, 0.0f );
			start = i + 1;
		}
	}

	// Draw the last line.
	string line = text.substr( start );
	draw( 0.0f, 0.0f, line );

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
		char currentChar = text[i];
		if (currentChar == ' ' || currentChar == '\n') {
			const string& current = text.substr( start, i - start );

			RECT rect;
			measure( &rect, current );

			// Check whether this line exceeds bounds.
			bool has_drawn = false;
			if ((rect.right - rect.left) > width) {
				if (first) {
					start = i + 1;
					draw( 0.0f, 0.0f, current );
					has_drawn = true;
				}
				else {
					const string& last = text.substr( start, prev - start );
					draw( 0.0f, 0.0f, last );
					start = prev + 1;
					has_drawn = true;
				}
			}
			else {
				first = false;
			}

			if (has_drawn) {
				glTranslatef( 0.0f, face_->size->metrics.height >> 6, 0.0f );
				new_lines++;
			}

			prev = i;
		}
	}

	// Finish the last line.
	const string& final = text.substr( start );
	RECT rect = {0, 0, 0, 0};
	measure( &rect, final );
	if ((rect.right - rect.left) > width && !first) {
		if (!first) {
			// Not the first word, draw last and then first.
			const string& last = text.substr( start, prev - start );
			draw( 0.0f, 0.0f, last );

			// Finish last.
			glTranslatef( 0.0f, face_->size->metrics.height >> 6, 0.0f );
			const string& end = text.substr( prev + 1 );
			draw( 0.0f, 0.0f, end );

			new_lines++;
		}
		else {
			// TODO: Need to split the word.
			draw( 0.0f, 0.0f, final );
		}
	}
	else {
		draw( 0.0f, 0.0f, final );
	}

	// Adjust rect bounds by this size.
	bounds->bottom = bounds->top + (new_lines * get_baseline_spacing()) + get_line_height();
	glPopMatrix();
	glEndList();
}

GLsizei Font::get_line_height() const
{
	return face_->size->metrics.ascender >> 6;
}

GLsizei Font::get_baseline_spacing() const
{
	return face_->size->metrics.height >> 6;
}