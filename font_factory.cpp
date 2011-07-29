#include "font_factory.h"
#include "curl.h"

FT_Library FontFactory::library_ = nullptr;

void FontFactory::initialize()
{
	// Load library.
	if (library_ == nullptr) {
		FT_Error error = FT_Init_FreeType( &library_ );
		if (error != 0) {
			throw Exception( "Failed to initialize FreeType library." );
		}
	}
}

void FontFactory::shut_down()
{
	// Unload library.
	if (library_ != nullptr) {
		FT_Done_FreeType( library_ );
		library_ = nullptr;
	}
}

Font* FontFactory::create_font( const std::string& filename, FT_F26Dot6 height )
{
	// Create font.
	FT_Face face;
	FT_Error error = FT_New_Face( library_, filename.c_str(), 0, &face );
	if (error != FT_Err_Ok) {
		try {
			Curl* curl = Curl::get_instance();
			curl->download( "http://www.jengerer.com/item_manager/" + filename, filename );
		}
		catch (const Exception& ex) {
			throw ex;
		}
		
		error = FT_New_Face( library_, filename.c_str(), 0, &face );
		if (error != 0) {
			throw Exception( "Failed to load font face." );
		}
	}

	// Set character height.
	FT_Set_Char_Size( face, 0, height << 6, 96, 96 );

	// Create font.
	Font* font = new Font( face );
	font->generate_glyphs();

	// Return face.
	FT_Done_Face( face );
	return font;
}