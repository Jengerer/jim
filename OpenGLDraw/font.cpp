#include "font.h"

Font::Font( const string& fontPath, float size )
{
	face_ = new OGLFT::Monochrome( fontPath.c_str(), size );
}