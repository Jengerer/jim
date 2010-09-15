#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <string>

#include "Object.h"

using namespace std;

/*
 * Texture class that handles DirectX
 * textures to be drawn.
 */
class Texture: public Object
{
public:
	Texture(LPDIRECT3DTEXTURE9 newTexture, 
		string newFilename,
		D3DXIMAGE_INFO newInfo);
	virtual ~Texture();

	/* Handling resources. */
	void setTexture(LPDIRECT3DTEXTURE9 newTexture, string newFilename);
	void releaseTexture();
	bool isLoaded() const;

	/* Getter functions. */
	LPDIRECT3DTEXTURE9 getTexture();
	string getFilename() const;
	int getWidth() const;
	int getHeight() const;

private:
	/* The texture and file path. */
	LPDIRECT3DTEXTURE9	m_lpTexture;
	string				m_strTexture;

	D3DXIMAGE_INFO		m_d3dInfo;
};