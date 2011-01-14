#include "Texture.h"

Texture::Texture(
	LPDIRECT3DTEXTURE9 texture,
	const string& filename,
	D3DXIMAGE_INFO info )
{
	setTexture(texture, filename);
	info_ = info;
}

Texture::~Texture()
{
	//Texture has been destroyed.
	releaseTexture();
}

void Texture::setTexture( LPDIRECT3DTEXTURE9 texture, const string& filename )
{
	texture_	= texture;
	filename_	= filename;
}

void Texture::releaseTexture()
{
	if (texture_)
	{
		texture_->Release();
		texture_ = 0;
	}
}

bool Texture::isLoaded() const
{
	return (texture_ != 0);
}

int Texture::getWidth() const
{
	return info_.Width;
}

int Texture::getHeight() const
{
	return info_.Height;
}

LPDIRECT3DTEXTURE9 Texture::getTexture()
{
	return texture_;
}

const string& Texture::getFilename() const
{
	return filename_;
}