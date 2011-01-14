#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <string>

using namespace std;

/*
 * Texture class that handles DirectX
 * textures to be drawn.
 */
class Texture
{
public:
	Texture( IDirect3DTexture9 *texture, 
		const string& filename,
		D3DXIMAGE_INFO info );
	~Texture();

	// Handling resources.
	void setTexture( IDirect3DTexture9 *texture, const string& filename );
	void releaseTexture();
	bool isLoaded() const;

	// Getter functions.
	LPDIRECT3DTEXTURE9	getTexture();
	const string&		getFilename() const;
	int					getWidth() const;
	int					getHeight() const;

private:
	// Texture and filename.
	IDirect3DTexture9	*texture_;
	string				filename_;

	// Imagine information.
	D3DXIMAGE_INFO		info_;
};