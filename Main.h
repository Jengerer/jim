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
	Main(const char* title,
		HINSTANCE hInstance,
		int width,
		int height);
	~Main();

	virtual void	openInterfaces() = 0;
	virtual void	closeInterfaces() = 0;

	// Getting window.
	Window*			getWindow() const;

	// Main running functions.
	virtual void	onFrame() = 0;

	// TODO: Make a better keyboard input interface.
	bool			isKeyUp( const int keyCode );
	bool			isKeyDown( const int keyCode );

protected:
	Window*			window_;
};