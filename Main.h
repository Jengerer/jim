#pragma once

#include "Window.h"
#include "MouseListener.h"
#include <string>
#include <sstream>

using namespace std;

// Function prototype.
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

/* The main class that controls all elements
 * of the application. */
class Main: public MouseListener
{
public:
	Main(char* newTitle,
		HINSTANCE hInstance,
		int newWidth,
		int newHeight);
	~Main();

	virtual void loadInterfaces() = 0;
	virtual void closeInterfaces() = 0;

	// Getting window.
	Window*				getWindow() const;

	// Main running functions.
	virtual void		onFrame() = 0;

	// Input functions.
	virtual void		onMouseUp() = 0;
	virtual void		onMouseDown() = 0;

	// TODO: Make a better keyboard input function.
	bool				keyUp(const int keyCode);
	bool				keyDown(const int keyCode);

protected:
	Window*				m_pWindow;
};