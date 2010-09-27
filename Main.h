#pragma once

#include <string>
#include <sstream>
#include <vector>

#include "Window.h"
#include "MouseListener.h"

using namespace std;

// Message handler prototype.
LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Main class that's the base of all applications.
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

	// Mouse listener methods.
	void			addMouseListener( MouseListener* mouseListener );

	// Getting window.
	Window*			getWindow() const;

	// Main running functions.
	virtual void	onFrame() = 0;

	// TODO: Make a better keyboard input interface.
	bool			isKeyUp( const int keyCode );
	bool			isKeyDown( const int keyCode );

protected:
	Window*			window_;
	vector<MouseListener*> mouseListeners_;
};