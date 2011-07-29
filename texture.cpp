#include "texture.h"

Texture::Texture()
{
	set_texture( 0, 0, 0, 1.0f, 1.0f );
}

Texture::Texture( GLuint texture, GLsizei width, GLsizei height, GLfloat tu, GLfloat tv )
{
	set_texture( texture, width, height, tu, tv );
}

Texture::~Texture()
{
	glDeleteTextures( 1, &texture_ );
	texture_ = 0;
}

void Texture::set_texture( GLuint texture, GLsizei width, GLsizei height, GLfloat tu, GLfloat tv )
{
	texture_ = texture;
	width_ = width;
	height_ = height;
	tu_ = tu;
	tv_ = tv;
}

bool Texture::is_loaded() const
{
	return texture_ != 0;
}

GLuint Texture::get_texture() const
{
	return texture_;
}

GLsizei Texture::get_width() const
{
	return width_;
}

GLsizei Texture::get_height() const
{
	return height_;
}

GLfloat Texture::get_tu() const
{
	return tu_;
}

GLfloat Texture::get_tv() const
{
	return tv_;
}