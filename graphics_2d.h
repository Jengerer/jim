#ifndef GRAPHICS_2D_H
#define GRAPHICS_2D_H

#include "colour.h"
#include "file_texture.h"
#include "window.h"
#include "opengl_shared.h"
#include "string_hasher.h"

#include <hash_map>
#include <string>
#include <stdexcept>

typedef std::hash_map<std::string, FileTexture*, StringHasher> TextureMap;

class Graphics2D
{

public:

	Graphics2D( Window* window );
	~Graphics2D( void );

	// Loading/closing interfaces.
	void initialize( void );
	void clean_up( void );

	// Scene initialization.
	void setup_scene();
	void resize_scene( GLsizei width, GLsizei height );

	// Buffer handling.
	void clear_scene();
	void swap_buffers();

	// Resource handling.
	Texture*		create_empty_texture( GLsizei width, GLsizei height, GLenum format );
	GLuint			create_texture( GLubyte* data, GLsizei width, GLsizei height, GLenum format );
	FileTexture*	get_texture( const std::string& filename );
	FileTexture*	get_texture( const std::string& filename, const std::string& url );

	// Drawing functions.
	void draw_rectangle( GLfloat x, GLfloat y, GLfloat width, GLfloat height );
	void draw_rounded_rect( GLfloat x, GLfloat y, GLfloat width, GLfloat height, GLfloat radius );
	void draw_texture( const Texture* texture, GLfloat x, GLfloat y );
	void draw_texture( const Texture* texture, GLfloat x, GLfloat y, GLsizei width, GLsizei height );
	void draw_display_list( GLuint list, GLfloat x, GLfloat y );

	// Setting render context.
	HGLRC get_render_context() const;
	HGLRC get_loading_context() const;
	bool set_render_context( HGLRC context );
	bool unset_render_context();

	// Render mode handling.
	void set_blend_state( GLenum src_blend, GLenum dest_blend );
	void set_colour( const Colour& colour );

	// Framebuffer handling.
	void render_to_texture( const Texture* texture );
	void reset_render_target();

private:

	// Resource handling.
	void load_texture( FileTexture* texture );

public:

	HGLRC loading_rc_;
	HDC dc_;

private:

	// Window variables.
	Window* window_;

	// OpenGL contexts.
	HGLRC rc_;

	// Framebuffer variables.
	GLuint fbo_;

	// Texture map.
	TextureMap textures_;
	FileTexture* rounded_corner_;

};

#endif // GRAPHICS_2D_H