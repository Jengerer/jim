#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <string>

using namespace std;

class Texture
{

public:

	Texture( const string& filename );
	~Texture( void );

	// Handling resources.
	void				SetTextureFilename( const string& filename );
	const string&		GetTextureFilename( void ) const;
	void				SetTexture( IDirect3DTexture9 *texture );
	void				SetTexture( IDirect3DTexture9 *texture, const D3DXIMAGE_INFO& info );
	IDirect3DTexture9*	GetTexture( void );
	void				ReleaseTexture( void );

	bool				IsLoaded( void ) const;

	// Texture attribute functions.
	int					GetWidth( void ) const;
	int					GetHeight( void ) const;

private:

	IDirect3DTexture9	*texture_;
	string				filename_;
	D3DXIMAGE_INFO		info_;

};