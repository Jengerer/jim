#include "texture.h"

Texture::Texture( const string& filename )
{
	SetTexture( nullptr );
	SetTextureFilename( filename );
}

Texture::~Texture()
{
	//Texture has been destroyed.
	ReleaseTexture();
}

void Texture::SetTextureFilename( const string& filename )
{
	filename_ = filename;
}

const string& Texture::GetTextureFilename( void ) const
{
	return filename_;
}

void Texture::SetTexture( IDirect3DTexture9 *texture )
{
	texture_ = texture;
}

void Texture::SetTexture( IDirect3DTexture9 *texture, const D3DXIMAGE_INFO& info )
{
	SetTexture( texture );
	info_ = info;
}

IDirect3DTexture9* Texture::GetTexture( void )
{
	return texture_;
}

void Texture::ReleaseTexture( void )
{
	if (texture_ != nullptr) {
		texture_->Release();
		texture_ = nullptr;
	}
}

bool Texture::IsLoaded( void ) const
{
	return (texture_ != nullptr);
}

int Texture::GetWidth( void ) const
{
	return info_.Width;
}

int Texture::GetHeight( void ) const
{
	return info_.Height;
}