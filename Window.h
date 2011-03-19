#pragma once

#include <d3d9.h>
#include "Exception.h"

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
	
	int		getWidth() const;
	int		getHeight() const;
	HWND	getHandle() const;


private:
	HWND		hWnd_;
	const char*	title_;
	int			width_, height_;
};