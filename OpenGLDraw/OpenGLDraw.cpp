#include "exception.h"
#include "graphics_2d.h"
#include "window.h"

#include <windows.h>
#include <OGLFT.h>

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

#include <sstream>
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

	OGLFT::Texture* font = new OGLFT::TranslucentTexture( "tf2build.ttf", 16 );
	font->setForegroundColor( 1.0f, 0.0f, 0.0f, 1.0f );
	OGLFT::BBox box = font->measure( "this is a message" );
	DWORD start = GetTickCount();
	float frames = 0;

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
			frames++;
			stringstream text;
			float framerate = frames / (static_cast<float>(GetTickCount() - start) / 1000.0f);
			text << framerate;
			graphics->BeginScene();
			font->draw( 400 - (box.x_max_ - box.x_min_) / 2, 240 - (box.y_max_ - box.y_min_) / 2, text.str().c_str());
			graphics->EndScene();
			glPopAttrib();
		}
	}

	delete graphics;
	delete window;

	return EXIT_SUCCESS;
}