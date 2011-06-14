#pragma once

#include <d3d9.h>
#include "exception.h"

class Window
{
public:
	//Class constructor.
	Window( HINSTANCE hInstance,
		const char* title,
		int width, int height );
	~Window();

	bool	registerClass( HINSTANCE hInstance );
	void	createWindow( HINSTANCE hInstance );

	bool	isActive();
	
	int		GetWidth() const;
	int		GetHeight() const;
	HWND	getHandle() const;


private:
	HWND		hWnd_;
	const char*	title_;
	int			width_, height_;
};