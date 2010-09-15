#include "ItemManager.h"

//TODO: Change screen depending on mouse position.

#define	APPLICATION		"Jengerer's Item Manager"
#define	VERSION			1000
#define	FRAMESPEED		1000/30

#define WIDTH			800
#define HEIGHT			500

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