#include "Main.h"

/* Main constructor. */
Main::Main(
	const char* title,
	HINSTANCE hInstance,
	int width, int height ) : Container( 0.0f, 0.0f )
{
	// Create and initialize a window.
	window_ = new Window(
		hInstance,
		wndProc,
		title,
		width, height );
}

Main::~Main()
{
	if (window_ != NULL)
	{
		delete window_;
		window_ = NULL;
	}
}

void Main::exitApplication()
{
	PostMessage( getWindow()->getHandle(), WM_DESTROY, 0, 0 );
}

Window* Main::getWindow() const
{
	return window_;
}

int Main::getWidth() const
{
	return window_->getWidth();
}

int Main::getHeight() const
{
	return window_->getHeight();
}

bool Main::keyPressed( const int keyCode )
{
	return ((GetAsyncKeyState( keyCode ) & 0x8000) != 0);
}