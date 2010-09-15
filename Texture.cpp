#include "Texture.h"

Texture::Texture(LPDIRECT3DTEXTURE9 newTexture,
				 string newFilename,
				 D3DXIMAGE_INFO newInfo)
{
	setTexture(newTexture, newFilename);
	m_d3dInfo = newInfo;
}

Texture::~Texture()
{
	//Texture has been destroyed.
	releaseTexture();
}

void Texture::setTexture(LPDIRECT3DTEXTURE9 newTexture, string newFilename)
{
	m_lpTexture = newTexture;
	m_strTexture = newFilename;
}

void Texture::releaseTexture()
{
	if (m_lpTexture != NULL)
	{
		m_lpTexture->Release();
		m_lpTexture = NULL;
	}
}

bool Texture::isLoaded() const
{
	return (m_lpTexture != NULL);
}

int Texture::getWidth() const
{
	return m_d3dInfo.Width;
}

int Texture::getHeight() const
{
	return m_d3dInfo.Height;
}

LPDIRECT3DTEXTURE9 Texture::getTexture()
{
	return m_lpTexture;
}

string Texture::getFilename() const
{
	return m_strTexture;
}