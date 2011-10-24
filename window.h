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

	bool	register_class( HINSTANCE hInstance );
	void	create_window( HINSTANCE hInstance );

	bool	is_active();
	
	int		get_width() const;
	int		get_height() const;
	HWND	get_handle() const;


private:

	HWND		hWnd_;
	const char*	title_;
	int			width_, height_;

};

#endif // WINDOW_H