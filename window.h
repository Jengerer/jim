#ifndef WINDOW_H
#define WINDOW_H

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdexcept>

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

#endif // WINDOW_H