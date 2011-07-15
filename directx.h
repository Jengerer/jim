#ifndef DIRECT_X_H
#define DIRECT_X_H

#include <string>
#include <sstream>
#include <hash_map>

#include "curl.h"
#include "texture.h"
#include "exception.h"
#include "string_hasher.h"
#include "window.h"
#include "font.h"

using namespace std;

struct TextureVertex {
	float x, y, z;
	DWORD colour;
	float tu, tv;
};

struct DiffuseVertex {
	float x, y, z;
	DWORD colour;
};

typedef std::hash_map<string, Texture*, StringHasher> TextureMap;

#define D3D9T_DIFFUSEVERTEX ( D3DFVF_XYZ | D3DFVF_DIFFUSE )
#define D3D9T_TEXTUREVERTEX ( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 )

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

	// DirectX post-initialization.
	void		SetProjectionSize( float width, float height );
	void		ResetProjectionSize( void );

	// Resource handling.
	void		LoadTextures( void );
	void		FreeTextures( void );
	void		ReleaseTextures( void );
	Texture*	GetTexture( const string& filename );
	Texture*	GetTexture( const string& filename, const string& url );

	// Blending states.
	void		SetBlendState( DWORD sourceBlend, DWORD destBlend );

	// Texture drawing and rendering.
	Texture*	CreateTexture( int width, int height );
	void		SetRenderTarget( Texture *texture );
	void		ResetRenderTarget( void );

	// Font handling.
	Font*		CreateFont( const string& name, int height, bool isBolded );

	// Vertex buffer.
	void		CreateDiffuseBuffer( void );
	void		CreateTextureBuffer( void );
	void		DrawColouredQuad( float x, float y, int width, int height, D3DCOLOR colour );
	void		DrawTexture( Texture* texture, float x, float y, int width, int height, D3DCOLOR colour );
	void		DrawRoundedRect( float x, float y, int width, int height, float radius, D3DCOLOR colour );
	void		DrawRoundedRect( float x, float y, int width, int height, float radiusTl, float radiusTr, float radiusBr, float radiusBl, D3DCOLOR colour );

	// Transformations.
	void		GetWorldTransform( D3DXMATRIX *matrix );
	void		SetWorldTransform( D3DXMATRIX *matrix );

	// Running functions.
	bool		CheckDevice( void );
	bool		BeginDraw( void );
	void		EndDraw( void );

private:

	void		LoadTexture( Texture* texture );

private:

	// Direct3D interfaces.
	IDirect3D9					*d3d_;
	IDirect3DDevice9			*d3dDevice_;
	IDirect3DSurface9			*backBuffer_;
	D3DPRESENT_PARAMETERS		params_;

	// Transformation matrices.
	D3DXMATRIX					matWorld_;
	D3DXMATRIX					matDraw_;

	// Vertex drawing.
	IDirect3DVertexBuffer9		*textureBuffer_;
	IDirect3DVertexBuffer9		*diffuseBuffer_;
	Texture						*roundedCorner_;

	// Texture handling.
	TextureMap					*textures_;

};

#endif // DIRECT_X_H