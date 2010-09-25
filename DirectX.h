#pragma once

#include "Main.h"
#include "Curl.h"
#include "Texture.h"
#include "Hashtable.h"
#include "Exception.h"

#include <string>
#include <sstream>

using namespace std;

class DirectX: public Main, public Curl
{
public:
	DirectX( const char* title,
		HINSTANCE hInstance,
		int width,
		int height );
	~DirectX();

	// Initializing and closing.
	void openInterfaces();
	void closeInterfaces();

	// Resource handling.
	void loadTextures();
	void releaseTextures();
	Texture* getTexture( const string& filename );
	Texture* loadTexture( const string& filename );

	// Running functions.
	virtual void onRedraw() = 0;
	void redraw();
	bool checkDevice();
	bool beginDraw();
	void endDraw();

	// Drawing functions.
	void drawTexture( Texture* texture, float x, float y );
	void drawTexture( Texture* texture, float x, float y, const D3DCOLOR& colour);
	void drawText( const string& text, RECT* rect, const DWORD& format, const D3DCOLOR& colour );	
	void setTransform( const D3DXMATRIX* matrix );
	void setTransform( float x, float y, float rotation, float xScale, float yScale );

	// Window handling.
	Window* getWindow();
	int		getWidth() const;
	int		getHeight() const;

private:
	// Direct3D interfaces.
	LPDIRECT3D9					d3d_;
	LPDIRECT3DDEVICE9			d3ddev_;
	LPD3DXSPRITE				sprite_;

	// Text drawing.
	LPD3DXFONT					bodyFont_;

	// Present parameters.
	D3DPRESENT_PARAMETERS		params_;

	// Texture handling.
	Hashtable*					textures_;
};