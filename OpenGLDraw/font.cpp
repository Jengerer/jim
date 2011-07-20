#include "font.h"

Font::Font( FT_Face face )
{
	face_ = face;
	bitmaps_ = nullptr;
}

void Font::GenerateDrawList()
{
	// Create base draw list and textures.
	FT_Long numGlyphs = face_->num_glyphs;
	baseList_ = glGenLists( numGlyphs );
	glGenTextures( numGlyphs, &textures_ );

	// Allocate space for glyphs.
	FT_Error error;
	for (FT_UInt i = 0; i < face_->num_glyphs; ++i) {
		// Get glyph.
		error = FT_Load_Glyph( face_, i, FT_LOAD_DEFAULT );
		if (error != 0) {
			throw Exception( "Failed to load glyph." );
		}

		// Convert to bitmap.
		error = FT_Render_Glyph( face_->glyph, FT_RENDER_MODE_NORMAL );
		if (error != 0) {
			throw Exception( "Failed to render glyph." );
		}
	}
}

FT_Bitmap* Font::GetBitmapForChar( char ch )
{
	return nullptr;
}