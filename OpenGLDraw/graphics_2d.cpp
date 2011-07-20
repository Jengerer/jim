#include "graphics_2d.h"
#include <png.h>

// Helper function get next power of 2.
inline GLsizei next_power_of_2( GLsizei number )
{
	GLsizei result = 1;
	while (result < number) {
		result <<= 1;
	}

	return result;
}

Graphics2D::Graphics2D( Window* window )
{
	window_ = window;
	dc_ = nullptr;
	rc_ = nullptr;
}

Graphics2D::~Graphics2D( void )
{
	CloseInterfaces();
}

void Graphics2D::LoadInterfaces( void )
{
	PIXELFORMATDESCRIPTOR pixelDesc;
	ZeroMemory( &pixelDesc, sizeof( pixelDesc ) );
	pixelDesc.nSize = sizeof( PIXELFORMATDESCRIPTOR );
	pixelDesc.nVersion = 1;
	pixelDesc.dwFlags = PFD_DRAW_TO_WINDOW | 
		PFD_SUPPORT_OPENGL | 
		PFD_DOUBLEBUFFER;
	pixelDesc.iPixelType = PFD_TYPE_RGBA;
	pixelDesc.cColorBits = 16;
	pixelDesc.iLayerType = PFD_MAIN_PLANE;
	pixelDesc.cDepthBits = 16;

	dc_ = GetDC( window_->getHandle() );
	if (dc_ == nullptr) {
		throw Exception( "Failed to get device context for window." );
	}

	GLuint pixelFormat = ChoosePixelFormat( dc_, &pixelDesc );
	if (pixelFormat == 0) {
		throw Exception( "Failed to choose pixel format." );
	}

	if (!SetPixelFormat( dc_, pixelFormat, &pixelDesc )) {
		throw Exception( "Failed to set pixel format." );
	}

	rc_ = wglCreateContext( dc_ );
	if (rc_ == nullptr) {
		throw Exception( "Failed to create OpenGL context." );
	}

	if (!wglMakeCurrent( dc_, rc_ )) {
		throw Exception( "Failed to set current context." );
	}

	SetForegroundWindow( window_->getHandle() );
	SetFocus( window_->getHandle() );
	ResizeScene( window_->GetWidth(), window_->GetHeight() );
	InitializeGL();
}

void Graphics2D::CloseInterfaces()
{
	if (rc_ != nullptr) {
		if (!wglMakeCurrent( nullptr, nullptr )) {
			throw Exception( "Release of device and render context failed." );
		}

		if (!wglDeleteContext( rc_ )) {
			throw Exception( "Releasing render context failed." );
		}

		rc_ = nullptr;
	}

	if (dc_ != nullptr) {
		if (ReleaseDC( window_->getHandle(), dc_ ) == 0) {
			throw Exception( "Failed to release device context." );
		}
	}
}

Texture* Graphics2D::GetTexture( const string& name )
{
	return LoadTexture( name );
}

Texture* Graphics2D::CreateTexture( GLubyte* data, GLsizei width, GLsizei height, GLenum format )
{
	// First bump the size up.
	width = next_power_of_2( width );
	height = next_power_of_2( height );

	// Create texture.
	GLuint texture;
	glGenTextures( 1, &texture );
	glBindTexture( GL_TEXTURE_2D, texture );

	// Set texture filtering and wrap clamping.
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	// Get component count.
	GLint components;
	switch (format) {
	case GL_RGBA:
		components = 4;
		break;

	case GL_RGB:
		components = 3;
		break;

	case GL_LUMINANCE_ALPHA:
		components = 2;
		break;

	case GL_LUMINANCE:
		components = 1;
		break;

	default:
		throw Exception( "Unhandled format type." );
		break;
	}

	// Build mipmaps, delete buffer.
	gluBuild2DMipmaps( GL_TEXTURE_2D, components, width, height, format, GL_UNSIGNED_BYTE, data );
	return new Texture( texture, width, height );
}

Texture* Graphics2D::LoadTexture( const string& name )
{
	// Output variables.
	png_structp	png_ptr;
	png_infop	info_ptr;
	unsigned int sig_read = 0;
	FILE* fp = nullptr;

	// Attempt to open the file.
	if ((fp = fopen( name.c_str(), "rb" )) == nullptr) {
		throw Exception( "Failed to open file for reading." );
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
	for (png_uint_32 i = 0; i < padded_height; ++i) {
		void* startPtr = output + (4 * padded_width * i);
		if (i < height) {
			// Fill image and padd with 0.
			memcpy( startPtr, row_pointers[i], 4 * width );	
			memset( startPtr, 0, 4 * (padded_width - width) );
		}
		else {
			// Entire row 0.
			memset( startPtr, 0, 4 * padded_width );
		}
	}

	// Clean up.
	png_destroy_read_struct( &png_ptr, &info_ptr, nullptr );
	fclose( fp );
	
	Texture* texture = CreateTexture( output, width, height, GL_RGBA );
	delete[] output;

	// Return result.
	return texture;
}

void Graphics2D::InitializeGL()
{
	// Set background colour.
	glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
	glClearDepth( 1.0f );
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
}

void Graphics2D::ResizeScene( GLsizei width, GLsizei height )
{
	if (height == 0) {
		height = 1;
	}

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0.0, width, height, 0.0, 0.0, 1.0 );
	glMatrixMode( GL_MODELVIEW );
}

void Graphics2D::BeginScene()
{
	glClear( GL_COLOR_BUFFER_BIT );
}

void Graphics2D::EndScene()
{
	SwapBuffers( dc_ );
}

void Graphics2D::DrawRectangle( GLdouble x, GLdouble y, GLdouble width, GLdouble height )
{
	GLdouble x2 = x + width;
	GLdouble y2 = y + height;

	// Draw it.
	glColor3f( 1.0f, 0.0f, 0.0f );
	glBegin( GL_QUADS );
	glVertex2f( x, y );
	glVertex2f( x2, y );
	glVertex2f( x2, y2 );
	glVertex2f( x, y2 );
	glEnd();
}

void Graphics2D::DrawTexture( const Texture* texture, GLfloat x, GLfloat y )
{
	GLfloat x2 = x + texture->GetWidth();
	GLfloat y2 = y + texture->GetHeight();

	// Set texture.
	glBindTexture( GL_TEXTURE_2D, texture->GetTexture() );

	// Draw quad.
	glBegin( GL_QUADS );
		glTexCoord2f( 0.0f, 0.0f ); glVertex2f( x, y );
		glTexCoord2f( 1.0f, 0.0f ); glVertex2f( x2, y );
		glTexCoord2f( 1.0f, 1.0f );	glVertex2f( x2, y2 );
		glTexCoord2f( 0.0f, 1.0f );	glVertex2f( x, y2 );
	glEnd();
}