#include "texture.h"

Texture::Texture( GLuint texture, GLsizei width, GLsizei height )
{
	texture_ = texture;
	width_ = width;
	height_ = height;
}

Texture::~Texture()
{
	glDeleteTextures( 1, &texture_ );
}

GLuint Texture::GetTexture() const
{
	return texture_;
}

GLsizei Texture::GetWidth() const
{
	return width_;
}

GLsizei Texture::GetHeight() const
{
	return height_;
}