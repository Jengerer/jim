#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <string>

using namespace std;

// TODO: Create a parent class that doesn't require filename.
class Texture
{

public:

	Texture();
	Texture( const string& filename, const string& url );
	~Texture( void );

	// Handling resources.
	const string&		GetUrl() const;
	const string&		GetFilename( void ) const;
	void				SetTexture( IDirect3DTexture9 *texture );
	void				SetTexture( IDirect3DTexture9 *texture, const D3DXIMAGE_INFO& info );
	IDirect3DTexture9*	GetTexture( void );
	void				ReleaseTexture( void );

	bool				IsLoaded( void ) const;

	// Texture attribute functions.
	int					GetWidth( void ) const;
	int					GetHeight( void ) const;

private:

	void				SetUrl( const string& url );
	void				SetFilename( const string& filename );

private:

	IDirect3DTexture9	*texture_;
	string				filename_;
	string				url_;
	D3DXIMAGE_INFO		info_;

};