#pragma once

#include <d3d9.h>
#include "Exception.h"

class Window
{
public:
	//Class constructor.
	Window( HINSTANCE hInstance,
		WNDPROC wndProc, 
		const char* title,
		int width, int height );
	~Window();

	bool	registerClass( HINSTANCE hInstance, WNDPROC wndProc );
	void	createWindow( HINSTANCE hInstance );
	
	int		getWidth() const;
	int		getHeight() const;
	HWND	getHandle() const;
private:
	HWND		hWnd_;

	const char*	title_;
	int			width_, height_;
};