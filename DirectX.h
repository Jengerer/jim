#pragma once

#include "Main.h"
#include "Curl.h"
#include "Texture.h"
#include "Hashtable.h"
#include "Exception.h"
#include "MouseListener.h"
#include "Font.h"

#include <string>
#include <sstream>

using namespace std;

struct TextureVertex {
	float x, y, z, rhw;
	DWORD colour;
	float tu, tv;
};

struct ColourVertex {
	float x, y, z, rhw;
	DWORD colour;
};

#define D3D9T_TEXTUREVERTEX (D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_DIFFUSE)
#define D3D9T_COLOURVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

// Prototypes.
class Font;

class DirectX: public Main, public Curl
{
public:
	DirectX( const char* title,
		HINSTANCE hInstance,
		int width,
		int height );
	virtual ~DirectX();

	// Initializing and closing.
	void openInterfaces();
	void closeInterfaces();

	// Font handling.
	Font* createFont( const string& name, int height, bool isBolded );

	// Resource handling.
	void loadTextures();
	void freeTextures();
	void releaseTextures();
	Texture* getTexture( const string& filename );
	Texture* loadTexture( const string& filename );

	// Vertex buffer.
	void createTexturedQuad( TextureVertex *vertices, float x, float y, int width, int height, D3DCOLOR colour = 0xFFFFFFFF );
	void createColouredQuad( ColourVertex *vertices, float x, float y, int width, int height, D3DCOLOR colour );
	void drawColouredQuad( void* vertices, size_t verticesSize );
	void drawTexturedQuad( TextureVertex *vertices, Texture* texture );
	void drawRoundedRect( float x, float y, int width, int height, float radius, D3DCOLOR colour );

	// Running functions.
	virtual void onRedraw() = 0;
	void redraw();
	bool checkDevice();
	bool beginDraw();
	void endDraw();

	// Drawing functions.
	void drawTexture( Texture* texture, float x, float y, float width, float height, const D3DCOLOR& colour = D3DCOLOR_XRGB( 255, 255, 255 ));
	void drawText( const string& text, RECT* rect, const DWORD& format, const D3DCOLOR& colour );	
	void setTransform( const D3DXMATRIX* d3dMatrix );
	void setTransform( float x, float y, float rotation, float xScale, float yScale );

	// Window handling.
	Window* getWindow();
	int		getWidth() const;
	int		getHeight() const;

private:
	// Direct3D interfaces.
	IDirect3D9					*d3d_;
	IDirect3DDevice9			*d3dDevice_;
	ID3DXSprite					*sprite_;

	// Vertex drawing.
	TextureVertex				texVertices_[4];
	ColourVertex				clrVertices_[4];
	IDirect3DVertexBuffer9		*vertexBuffer_;
	IDirect3DVertexBuffer9		*colourBuffer_;
	Texture						*roundedCorner_;

	// Text drawing.
	Font						*bodyFont_;
	Font						*labelFont_;

	// Present parameters.
	D3DPRESENT_PARAMETERS		params_;

	// Texture handling.
	Hashtable*					textureMap_;
};