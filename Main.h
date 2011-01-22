#pragma once

#include <string>
#include <sstream>
#include <vector>

#include "Container.h"
#include "Window.h"
#include "Mouse.h"

using namespace std;

// Message handler prototype.
LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Main class that's the base of all applications.
class Main: public Container
{
public:
	Main(const char* title,
		HINSTANCE hInstance,
		int width,
		int height);
	virtual ~Main();

	// Initializing/closing.
	virtual void	openInterfaces() = 0;
	virtual void	closeInterfaces() = 0;
	void			exitApplication();

	// Getting window.
	Window*			getWindow() const;

	// Component size getters.
	virtual int		getWidth() const;
	virtual int		getHeight() const;

	// Main running functions.
	virtual void	run() = 0;

	// TODO: Make a better keyboard input interface.
	bool			keyPressed( const int keyCode );

protected:
	Window*			window_;
};