#include "exception.h"
#include "graphics_2d.h"
#include "window.h"

#include <windows.h>
#include "font.h"

Window* window = nullptr;
Graphics2D* graphics = nullptr;

inline GLsizei next_power2( GLsizei power2 )
{
	GLsizei result = 1;
	while (result < power2) {
		result = result << 1;
	}

	return result;
}

LRESULT CALLBACK wndProc( HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch (msg) {
	case WM_CLOSE:
	case WM_DESTROY:
		PostQuitMessage( 0 );
		return 0;
	}

	return DefWindowProc( wnd, msg, wParam, lParam );
}

void RenderScene( void )
{
	glClear( GL_COLOR_BUFFER_BIT );
	glBegin( GL_TRIANGLES );
		glVertex3f( -0.5f, -0.5f, 0.0f );
		glVertex3f( 0.5f, 0.0f, 0.0f );
		glVertex3f( 0.0f, 0.5f, 0.0f );
	glEnd();
	glutSwapBuffers();
}

int WINAPI WinMain( HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow )
{
	try {
		window = new Window( instance, "OpenGL Draw", 800, 480 );
		graphics = new Graphics2D( window );
		graphics->LoadInterfaces();
	}
	catch( const Exception& windowEx )
	{
		const char* lol = windowEx.getMessage()->c_str();
		MessageBox( NULL, windowEx.getMessage()->c_str(), "Initialization failed.", MB_OK );
	}

	// Load texture.
	Texture* texture = nullptr;
	try {
		texture = graphics->GetTexture( "test.png" );
	}
	catch (const Exception& ex) {
		MessageBox( NULL, ex.getMessage()->c_str(), "Load Failed", MB_OK );
		throw Exception( "Image load failed." );
	}

	// Start library.
	FT_Library library;
	FT_Error error = FT_Init_FreeType( &library );
	if (error != 0) {
		throw Exception( "Failed to start FreeType library." );
	}

	// Create font.
	FT_Face face;
	error = FT_New_Face( library,
		"tf2build.ttf",
		0,
		&face );
	if (error == FT_Err_Unknown_File_Format) {
		throw Exception( "Unexpected font format." );
	}
	else if (error != 0) {
		throw Exception( "Failed to load font." );
	}

	error = FT_Set_Char_Size(
		face,
		0,
		16 * 64,
		300, 300 );
	if (error != 0) {
		throw Exception( "Failed to set size." );
	}

	FT_UInt index = FT_Get_Char_Index( face, 'a' );
	error = FT_Load_Glyph( face, index, FT_LOAD_DEFAULT );
	if (error != 0) {
		throw Exception( "Failed to load glyph." );
	}

	error = FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL );
	if (error != 0) {
		throw Exception( "Failed to render glyph." );
	}

	FT_Bitmap* bitmap = &face->glyph->bitmap;

	bool running = true;
	MSG msg;
	while (running) {
		if (PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE )) {
			if (msg.message == WM_QUIT) {
				running = false;
			}
			else {
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
		}
		
		if (running) {
			graphics->BeginScene();
			glColor3f( 1.0f, 0.0f, 0.0f );
			glTexImage2D( GL_TEXTURE_2D, 0, 3, bitmap->width, bitmap->rows, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, bitmap->buffer );
			glBegin( GL_QUADS );
				glTexCoord2f( 0.0f, 0.0f ); glVertex2f( 100.0f, 100.0f );
				glTexCoord2f( 0.0f, 1.0f ); glVertex2f( 100.0f, 200.0f );
				glTexCoord2f( 1.0f, 1.0f ); glVertex2f( 200.0f, 200.0f );
				glTexCoord2f( 1.0f, 0.0f ); glVertex2f( 200.0f, 100.0f );
			glEnd();
			//glColor3f( 1.0f, 1.0f, 1.0f );
			//graphics->DrawTexture( texture, 10.0f, 10.0f );
			graphics->EndScene();
		}
	}

	delete graphics;
	delete window;

	return EXIT_SUCCESS;
}