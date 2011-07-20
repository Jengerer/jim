#ifndef TEXTURE_H
#define TEXTURE_H

#include "opengl_shared.h"

class Texture
{

public:

	Texture( GLuint texture, GLsizei width, GLsizei height );
	virtual ~Texture();

	GLuint GetTexture() const;
	GLsizei GetWidth() const;
	GLsizei GetHeight() const;

private:

	GLuint texture_;
	GLsizei width_;
	GLsizei height_;

};

#endif // TEXTURE_H