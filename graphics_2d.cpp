#include "graphics_2d.h"
#include "curl.h"
#include <png.h>

Graphics2D::Graphics2D( Window* window )
{
	window_ = window;
	dc_ = nullptr;
	rc_ = nullptr;

	// Null the rounded corner texture.
	rounded_corner_ = nullptr;
}

Graphics2D::~Graphics2D( void )
{
	clean_up();
}

void Graphics2D::initialize( void )
{
	// Fill out pixel format.
	PIXELFORMATDESCRIPTOR pixelDesc;
	ZeroMemory( &pixelDesc, sizeof( pixelDesc ) );
	pixelDesc.nSize = sizeof( PIXELFORMATDESCRIPTOR );
	pixelDesc.nVersion = 1;
	pixelDesc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pixelDesc.iPixelType = PFD_TYPE_RGBA;
	pixelDesc.cColorBits = 16;
	pixelDesc.iLayerType = PFD_MAIN_PLANE;
	pixelDesc.cDepthBits = 16;

	// Get device context.
	dc_ = GetDC( window_->getHandle() );
	if (dc_ == nullptr) {
		throw Exception( "Failed to get device context for window." );
	}

	// Choose pixel format.
	GLuint pixelFormat = ChoosePixelFormat( dc_, &pixelDesc );
	if (pixelFormat == 0) {
		throw Exception( "Failed to choose pixel format." );
	}

	// Set received pixel format.
	if (!SetPixelFormat( dc_, pixelFormat, &pixelDesc )) {
		throw Exception( "Failed to set pixel format." );
	}

	// Create GLRC handle.
	rc_ = wglCreateContext( dc_ );
	if (rc_ == nullptr) {
		throw Exception( "Failed to create OpenGL context." );
	}

	// Creating loading GLRC handle.
	loadRc_ = wglCreateContext( dc_ );
	if (loadRc_ == nullptr) {
		throw Exception( "Failed to create loading OpenGL context." );
	}

	// Set current contexts.
	if (!wglMakeCurrent( dc_, rc_ )) {
		throw Exception( "Failed to set current context." );
	}

	// Get rounded corner texture.
	rounded_corner_ = get_texture( "manager/rounded_corner" );

	// Create framebuffer objects.
	glGenFramebuffersEXT( 1, &fbo_ );

	// Show window.
	SetForegroundWindow( window_->getHandle() );
	SetFocus( window_->getHandle() );

	// Set scene up.
	setup_scene();
}

void Graphics2D::clean_up()
{
	// Empty texture map.
	for (auto i = textures_.begin(); i != textures_.end(); i = textures_.erase( i )) {
		delete i->second;
	}

	// Close rendering context.
	if (rc_ != nullptr) {
		// Unset contexts.
		if (!wglMakeCurrent( nullptr, nullptr )) {
			throw Exception( "Release of device and render context failed." );
		}

		// Delete rendering context.
		if (!wglDeleteContext( rc_ )) {
			throw Exception( "Releasing render context failed." );
		}

		rc_ = nullptr;
	}

	// Release device context.
	if (dc_ != nullptr) {
		if (ReleaseDC( window_->getHandle(), dc_ ) == 0) {
			throw Exception( "Failed to release device context." );
		}
	}
}

void Graphics2D::setup_scene()
{
	// Set background colour.
	glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
	glDisable( GL_DEPTH_TEST );

	// Enable alpha blending.
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	// Enable 2D textures.
	glEnable( GL_TEXTURE_2D );

	// Set pixel storing alignment.
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

	// Enable vertical-sync.
	if (GLEE_WGL_EXT_swap_control) {
		wglSwapIntervalEXT( 1 );
	}

	// Flat shading.
	glShadeModel( GL_FLAT );
	
	// Size scene to window.
	resize_scene( window_->GetWidth(), window_->GetHeight() );
}

void Graphics2D::resize_scene( GLsizei width, GLsizei height )
{
	glViewport( 0.0f, 0.0f, width, height );

	// Set up orthographic projection.
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluOrtho2D( 0.0f, width, height, 0.0f );

	// Set default model view.
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
}

FileTexture* Graphics2D::get_texture( const string& filename )
{
	return get_texture( filename, "http://www.jengerer.com/itemManager/img/" + filename + ".png" );
}

FileTexture* Graphics2D::get_texture( const string& filename, const string& url )
{
	auto i = textures_.find( filename );
	if (i != textures_.end()) {
		return i->second;
	}
	
	// Not found, load and insert.
	FileTexture* result = new FileTexture( filename, url );
	textures_[filename] = result;

	// NOTE: Inserted into table before load so cleanup works.
	load_texture( result );
	return result;
}

void Graphics2D::clear_scene()
{
	glClear( GL_COLOR_BUFFER_BIT );
}

void Graphics2D::swap_buffers()
{
	SwapBuffers( dc_ );
}

void Graphics2D::save_texture( Texture* destination, GLubyte* data, GLsizei width, GLsizei height, GLenum format )
{
	// First bump the size up.
	GLsizei real_width = next_power_of_2( width );
	GLsizei real_height = next_power_of_2( height );

	// Create texture.
	GLuint texture;
	glGenTextures( 1, &texture );
	glBindTexture( GL_TEXTURE_2D, texture );

	// Set texture filtering and wrap clamping.
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	// Get texture mapping edges.
	GLfloat tu = static_cast<float>(width) / static_cast<float>(real_width);
	GLfloat tv = static_cast<float>(height) / static_cast<float>(real_height);

	// Build mipmaps, delete buffer.
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, real_width, real_height, 0, format, GL_UNSIGNED_BYTE, data );
	glBindTexture( GL_TEXTURE_2D, 0 );
	destination->set_texture( texture, width, height, tu, tv );
}

Texture* Graphics2D::create_empty_texture( GLsizei width, GLsizei height, GLenum format )
{
	Texture* result = new Texture();
	save_texture( result, nullptr, width, height, format );
	return result;
}

void Graphics2D::load_texture( FileTexture* texture )
{
	// Get filename and URL.
	const string& filename = "img/" + texture->get_filename() + ".png";
	const string& url = texture->get_url();

	// Output variables.
	png_structp	png_ptr;
	png_infop	info_ptr;
	unsigned int sig_read = 0;
	
	// Attempt to open the file.
	FILE* fp = fopen( filename.c_str(), "rb" );
	if (fp == nullptr) {
		// Download the file.
		try {
			Curl* curl = Curl::get_instance();
			if (!curl->download( url, filename )) {
				throw Exception( "Failed to download " + filename + "." );
			}
		}
		catch (const Exception& ex) {
			throw ex;
		}

		// Re-attempt to read.
		fp = fopen( filename.c_str(), "rb");
		if (fp == nullptr) {
			throw Exception( "Failed to read " + filename + "." );
		}
	}

	// Read PNG struct.
	png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr );
	if (png_ptr == nullptr) {
		fclose( fp );
		throw Exception( "Failed to create PNG read struct." );
	}

	// Create PNG info struct.
	info_ptr = png_create_info_struct( png_ptr );
	if (info_ptr == nullptr) {
		fclose( fp );
		png_destroy_read_struct( &png_ptr, nullptr, nullptr );
		throw Exception( "Failed to create PNG info struct." );
	}

	// Initialize I/O.
	png_init_io( png_ptr, fp );
	png_set_sig_bytes( png_ptr, sig_read );

	// Begin read.
	png_read_png( png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, nullptr );

	// Get size and allocate.
	png_uint_32 width = png_get_image_width( png_ptr, info_ptr );
	png_uint_32 height = png_get_image_height( png_ptr, info_ptr );
	png_uint_32 padded_width = next_power_of_2( width );
	png_uint_32 padded_height = next_power_of_2( height );
	unsigned int allocSize = 4 * padded_width * padded_height;
	GLubyte* output = new GLubyte[ allocSize ];
	if (output == nullptr) {
		throw Exception( "Failed to allocate memory for PNG image." );
	}

	// Copy information.
	png_bytepp row_pointers = png_get_rows( png_ptr, info_ptr );
	for (png_uint_32 i = 0; i < height; ++i) {
		void* startPtr = output + (4 * padded_width * i);
		memcpy( startPtr, row_pointers[i], 4 * width );	
	}

	// Clean up.
	png_destroy_read_struct( &png_ptr, &info_ptr, nullptr );
	fclose( fp );
	
	// Generate texture from data.
	save_texture( texture, output, width, height, GL_RGBA );
	delete[] output;
}

void Graphics2D::draw_rectangle( GLfloat x, GLfloat y, GLfloat width, GLfloat height )
{
	GLfloat x2 = x + width;
	GLfloat y2 = y + height;

	// Draw it.
	glBegin( GL_QUADS );
	glVertex2f( x, y );
	glVertex2f( x2, y );
	glVertex2f( x2, y2 );
	glVertex2f( x, y2 );
	glEnd();
}

void Graphics2D::draw_rounded_rect( GLfloat x, GLfloat y, GLfloat width, GLfloat height, GLfloat radius )
{
	// Corners.
	draw_texture( rounded_corner_, x, y, radius, radius ); // Top-left.
	draw_texture( rounded_corner_, x + width, y, -radius, radius ); // Top-right.
	draw_texture( rounded_corner_, x + width, y + height, -radius, -radius ); // Bottom-right.
	draw_texture( rounded_corner_, x, y + height, radius, -radius ); // Bottom-left.

	// Double radius.
	GLfloat double_radius = radius * 2.0f;

	// Fill in center and tops.
	draw_rectangle( x, y + radius, width, height - double_radius ); // Middle.
	draw_rectangle( x + radius, y, width - double_radius, height ); // Top.
	draw_rectangle( x + radius, y + height - radius, width - double_radius, radius ); // Bottom.
}

void Graphics2D::draw_texture( const Texture* texture, GLfloat x, GLfloat y )
{
	draw_texture( texture, x, y, texture->get_width(), texture->get_height() );
}

void Graphics2D::draw_texture( const Texture* texture, GLfloat x, GLfloat y, GLsizei width, GLsizei height )
{
	// Set up end texture.
	GLfloat x2 = x + width;
	GLfloat y2 = y + height;

	// Set texture.
	glBindTexture( GL_TEXTURE_2D, texture->get_texture() );

	// Get texture coordinates.
	GLfloat tu = texture->get_tu();
	GLfloat tv = texture->get_tv();

	// Draw quad.
	glBegin( GL_QUADS );
		glTexCoord2f( 0.0f, 0.0f ); glVertex2f( x, y );
		glTexCoord2f( tu, 0.0f ); glVertex2f( x2, y );
		glTexCoord2f( tu, tv );	glVertex2f( x2, y2 );
		glTexCoord2f( 0.0f, tv ); glVertex2f( x, y2 );
	glEnd();

	// Unbind texture.
	glBindTexture( GL_TEXTURE_2D, 0 );
}

void Graphics2D::set_blend_state( GLenum src_blend, GLenum dest_blend )
{
	glBlendFunc( src_blend, dest_blend );
}

void Graphics2D::set_colour( const Colour& colour, GLubyte alpha )
{
	glColor4ub( colour.r, colour.g, colour.b, alpha );
}

void Graphics2D::render_to_texture( const Texture* texture )
{
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo_ );
	glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, 
		GL_TEXTURE_2D, texture->get_texture(), 0 );
	resize_scene( texture->get_width(), texture->get_height() );

	// Flip rendering for texture order.
	glMatrixMode( GL_PROJECTION );
	glScalef( 1.0f, -1.0f, 1.0f );
	glTranslatef( 0.0f, -texture->get_height(), 0.0f );
	glMatrixMode( GL_MODELVIEW );
}

void Graphics2D::reset_render_target()
{
	// Unbind texture/buffer.
	glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, 0, 0 );
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
	resize_scene( window_->GetWidth(), window_->GetHeight() );
}