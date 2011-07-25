#ifndef TEXTURE_H
#define TEXTURE_H

#include "opengl_shared.h"

class Texture
{

public:

	Texture();
	Texture( GLuint texture, GLsizei width, GLsizei height, GLfloat tu = 1.0f, GLfloat tv = 1.0f );
	virtual ~Texture();

	// Texture setter.
	void	set_texture( GLuint texture, GLsizei width, GLsizei height, GLfloat tu, GLfloat tv );

	// Texture attribute functions.
	bool	is_loaded() const;
	GLuint	get_texture() const;
	GLsizei	get_width() const;
	GLsizei	get_height() const;
	GLfloat	get_tu() const;
	GLfloat	get_tv() const;

private:

	GLuint texture_;

	// Size variables.
	GLsizei width_;
	GLsizei height_;

	// Texture mapping coordinates.
	GLfloat tu_;
	GLfloat tv_;

};

#endif // TEXTURE_H