#ifndef GRAPHICS_2D_H
#define GRAPHICS_2D_H

#include "exception.h"
#include "texture.h"
#include "window.h"
#include "opengl_shared.h"

class Graphics2D
{

public:

	Graphics2D( Window* window );
	~Graphics2D( void );

	void LoadInterfaces( void );
	void CloseInterfaces( void );

	void InitializeGL();
	void ResizeScene( GLsizei width, GLsizei height );

	void BeginScene();
	void EndScene();

	// Resource handling.
	Texture* CreateTexture( GLubyte* data, GLsizei width, GLsizei height, GLenum format );
	Texture* GetTexture( const string& file );

	void DrawRectangle( GLdouble x, GLdouble y, GLdouble width, GLdouble height );
	void DrawTexture( const Texture* texture, GLfloat x, GLfloat y );

private:

	// Resource handling.
	Texture* LoadTexture( const string& file );

private:

	Window* window_;

	HDC dc_;
	HGLRC rc_;

};

#endif // GRAPHICS_2D_H