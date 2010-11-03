#pragma once

#include <string>
#include <sstream>
#include <vector>

#include "Component.h"
#include "Window.h"
#include "MouseListener.h"

using namespace std;

// Message handler prototype.
LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Main class that's the base of all applications.
class Main: public Component, public MouseListener
{
public:
	Main(const char* title,
		HINSTANCE hInstance,
		int width,
		int height);
	virtual ~Main();

	// Initializing.
	virtual void	openInterfaces() = 0;
	virtual void	closeInterfaces() = 0;

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