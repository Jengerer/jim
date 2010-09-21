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
	Texture(LPDIRECT3DTEXTURE9 newTexture, 
		const string* newFilename,
		const string* newURL,
		D3DXIMAGE_INFO newInfo);
	virtual ~Texture();

	/* Handling resources. */
	void setTexture(LPDIRECT3DTEXTURE9 newTexture, const string* newFilename, const string* newURL);
	void releaseTexture();
	bool isLoaded() const;

	/* Getter functions. */
	LPDIRECT3DTEXTURE9 getTexture();
	const string* getFilename() const;
	const string* getURL() const;
	int getWidth() const;
	int getHeight() const;

private:
	/* The texture and file path. */
	LPDIRECT3DTEXTURE9	m_lpTexture;

	// File name and online path.
	const string*		m_textureName;
	const string*		m_textureURL;

	D3DXIMAGE_INFO		m_d3dInfo;
};