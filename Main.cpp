#include "Main.h"

/* Main constructor. */
Main::Main(char* newTitle,
		HINSTANCE hInstance,
		int newWidth,
		int newHeight)
{
	// Create and initialize a window.
	m_pWindow = new Window(
		hInstance,
		WndProc,
		newTitle,
		newWidth, newHeight);

	setWindow(m_pWindow);
}

/* Main deconstructor. */
Main::~Main()
{
	if (m_pWindow != NULL)
	{
		delete m_pWindow;
		m_pWindow = NULL;
	}
}

Window* Main::getWindow() const
{
	return m_pWindow;
}

bool Main::keyDown(const int keyCode)
{
	return GetAsyncKeyState(keyCode) & 0x8000;
}

bool Main::keyUp(const int keyCode)
{
	return !keyDown(keyCode);
}