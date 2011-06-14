#pragma once

#include <windows.h>
#include <string>

#include "directx.h"

using namespace std;

class Font
{
public:
	Font( ID3DXFont *d3dFont );
	virtual ~Font();

	void getTextRect( const string& text, LPRECT rect, DWORD format );
	void wrapText( string& text, LPRECT rect, DWORD format );
	void drawText( const string& text, LPRECT rect, DWORD format, const D3DCOLOR& colour );

private:
	ID3DXFont	*d3dFont_;
	int			height_;
};