#include "Font.h"

Font::Font( ID3DXFont *d3dFont )
{
	// Font created.
	d3dFont_ = d3dFont;
}

Font::~Font()
{
	// Font is destroyed.
	if (d3dFont_) {
		d3dFont_->Release();
		d3dFont_ = 0;
	}
}

void Font::getTextRect( const string& text, LPRECT rect, DWORD format )
{
	// Get font handle and select object.
	drawText( text, rect, format | DT_CALCRECT, 0 );
}

void Font::drawText(const string& text, LPRECT rect, DWORD format, const D3DCOLOR& colour )
{
	// Draw the text.
	d3dFont_->DrawTextA(
		0,
		text.c_str(),
		-1,
		rect,
		format,
		colour );
}