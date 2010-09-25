#include "Main.h"

/* Main constructor. */
Main::Main(char* newTitle,
		HINSTANCE hInstance,
		int newWidth,
		int newHeight)
{
	// Create and initialize a window.
	window_ = new Window(
		hInstance,
		WndProc,
		newTitle,
		newWidth, newHeight);

	setWindow(window_);
}

/* Main deconstructor. */
Main::~Main()
{
	if (window_ != NULL)
	{
		delete window_;
		window_ = NULL;
	}
}

Window* Main::getWindow() const
{
	return window_;
}

bool Main::keyDown(const int keyCode)
{
	return GetAsyncKeyState(keyCode) & 0x8000;
}

bool Main::keyUp(const int keyCode)
{
	return !keyDown(keyCode);
}