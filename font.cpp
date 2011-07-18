#include "font.h"

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
	drawText( text, rect, format | DT_CALCRECT, 0 );
}

void Font::wrapText( string& text, LPRECT bounds, DWORD format )
{
	size_t prev = 0;
	size_t next = 0;
	size_t start = 0;

	bool first = true;
	LONG width = bounds->right - bounds->left;

	for (size_t i = 0; i < text.length(); i++) {
		char currentChar = text[i];
		if (currentChar == ' ' || currentChar == '\n') {
			const string& current = text.substr( start, i - start );

			RECT rect = {0, 0, 0, 0};
			getTextRect( current, &rect, DT_SINGLELINE );

			// Check whether this line exceeds bounds.
			if ((rect.right - rect.left) > width) {
				if (first) {
					text[i] = '\n';
					start = i + 1;
				}
				else {
					text[prev] = '\n';
					start = prev + 1;
				}
			}
			else {
				first = false;
			}

			if (currentChar == '\n') {
				start = i + 1;
			}

			prev = i;
		}
	}

	// Add the last word.
	const string& final = text.substr( start );
	RECT rect = {0, 0, 0, 0};
	getTextRect( final, &rect, DT_SINGLELINE );
	if ((rect.right - rect.left) > (bounds->right - bounds->left)) {
		if (!first) {
			text[prev] = '\n';
		}
	}

	// Now get the height of the result text and size rect.
	RECT result = {0, 0, bounds->right - bounds->left, 0};
	getTextRect( text, &result, 0 );
	bounds->bottom = bounds->top + (result.bottom - result.top);
}

void Font::drawText(const string& text, LPRECT rect, DWORD format, const D3DCOLOR& colour )
{
	// Draw the text.
	MessageBox( NULL, "About to draw.", "Drawing Started", MB_OK );
	INT result = d3dFont_->DrawTextA(
		0,
		text.c_str(),
		-1,
		rect,
		format | DT_NOCLIP,
		colour );

	if (result == 0) {
		MessageBox( NULL, "Failed to draw text with D3DX font handle.", "Failed to draw.", MB_OK );
	}

	MessageBox( NULL, "Drawing finished.", "Drawing Ended", MB_OK );
}