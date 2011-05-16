#pragma once

#include <string>
#include <sstream>
#include <map>

#include "Curl.h"
#include "Texture.h"
#include "Exception.h"
#include "Window.h"
#include "Font.h"

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

// Bring string hasher in here.
typedef std::map<string, Texture*>	TextureMap;
typedef std::pair<string, Texture*>	TexturePair;

#define D3D9T_TEXTUREVERTEX (D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_DIFFUSE)
#define D3D9T_COLOURVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

// Prototypes.
class Font;
class DirectX: public Window, public Curl
{
public:
	DirectX( HINSTANCE instance,
		const char *title,
		int width, int height );
	virtual ~DirectX();

	// Initializing and closing.
	void		LoadInterfaces( void );
	void		CloseInterfaces( void );

	// Resource handling.
	void		LoadTextures( void );
	void		FreeTextures( void );
	void		ReleaseTextures( void );
	Texture*	GetTexture( const string& filename );
	void		LoadTexture( Texture* texture );

	// Texture drawing and rendering.
	Texture*	CreateTexture( const string& name, int width, int height );
	void		SetRenderTarget( Texture *texture );
	void		ResetRenderTarget( void );

	// Font handling.
	Font*		CreateFont( const string& name, int height, bool isBolded );

	// Vertex buffer.
	void		CreateTexturedQuad( TextureVertex *vertices, float x, float y, int width, int height, D3DCOLOR colour = 0xFFFFFFFF );
	void		CreateColouredQuad( ColourVertex *vertices, float x, float y, int width, int height, D3DCOLOR colour );
	void		DrawColouredQuad( void* vertices, size_t verticesSize );
	void		DrawTexturedQuad( TextureVertex *vertices, Texture* texture );
	void		DrawRoundedRect( float x, float y, int width, int height, float radius, D3DCOLOR colour );
	void		DrawRoundedRect( float x, float y, int width, int height, float radiusTl, float radiusTr, float radiusBr, float radiusBl, D3DCOLOR colour );

	// Running functions.
	bool		CheckDevice( void );
	bool		BeginDraw( void );
	void		EndDraw( void );

	// Drawing functions.
	void		DrawTexture( Texture* texture, float x, float y, float width, float height, const D3DCOLOR& colour = D3DCOLOR_XRGB( 255, 255, 255 ));

private:

	// Direct3D interfaces.
	IDirect3D9					*d3d_;
	IDirect3DDevice9			*d3dDevice_;
	IDirect3DSurface9			*backBuffer_;
	D3DPRESENT_PARAMETERS		params_;

	// Vertex drawing.
	TextureVertex				texVertices_[4];
	ColourVertex				clrVertices_[4];
	IDirect3DVertexBuffer9		*vertexBuffer_;
	IDirect3DVertexBuffer9		*colourBuffer_;
	Texture						*roundedCorner_;

	// Texture handling.
	TextureMap					*textures_;

};