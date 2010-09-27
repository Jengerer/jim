#include "Main.h"

/* Main constructor. */
Main::Main(
	const char* title,
	HINSTANCE hInstance,
	int width, int height )
{
	// Create and initialize a window.
	window_ = new Window(
		hInstance,
		wndProc,
		title,
		width, height );

	setWindow( window_ );
}

Main::~Main()
{
	if (window_ != NULL)
	{
		delete window_;
		window_ = NULL;
	}
}

void Main::addMouseListener( MouseListener* mouseListener )
{
	mouseListener->setWindow( window_ );
	mouseListeners_.push_back( mouseListener );
}

Window* Main::getWindow() const
{
	return window_;
}