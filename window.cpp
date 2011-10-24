#include "window.h"

// Prototype for window handling.
LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

Window::Window( HINSTANCE hInstance,
			   const char* title,
			   int width, int height )
{
	title_ = title;
	width_ = width;
	height_ = height;

	if (!register_class( hInstance )) {
		throw std::runtime_error( "Failed to register class." );
	}

	create_window( hInstance );
}

Window::~Window()
{
	//Window has been destroyed.
}

bool Window::register_class( HINSTANCE hInstance )
{
	WNDCLASS wndCls;

	wndCls.hInstance		= hInstance;
	wndCls.hIcon			= NULL;
	wndCls.hCursor			= LoadCursor( NULL, IDC_ARROW );
	wndCls.hbrBackground	= NULL;

	wndCls.cbWndExtra		= 0;
	wndCls.cbClsExtra		= 0;

	wndCls.lpfnWndProc		= wndProc;
	wndCls.lpszClassName	= title_;
	wndCls.lpszMenuName		= NULL;
	wndCls.style			= CS_OWNDC;

	return RegisterClass( &wndCls ) != 0;
}

void Window::create_window(HINSTANCE hInstance)
{
	// Get screen resolution to center window.
	RECT screenRect;
	GetWindowRect( GetDesktopWindow(), &screenRect );

	int x = (screenRect.right - get_width()) / 2;
	int y = (screenRect.bottom - get_height()) / 2;

	// Adjust bounds based on style.
	RECT windowRect;
	windowRect.left	= 0;
	windowRect.top = 0;
	windowRect.right = get_width();
	windowRect.bottom = get_height();

	// Adjust size for style.
	DWORD displayStyle = WS_CAPTION | WS_SYSMENU;
	if (!AdjustWindowRect( &windowRect, displayStyle, false )) {
		throw std::runtime_error( "Failed to adjust window rectangle." );
	}

	// Create window.
	hWnd_ = CreateWindow(
		title_, title_,
		displayStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		x, y,
		windowRect.right - windowRect.left, 
		windowRect.bottom - windowRect.top,
		NULL, NULL,
		hInstance,
		NULL );

	if (!hWnd_) {
		throw std::runtime_error( "Failed to create window." );
	}

	ShowWindow( hWnd_, SW_NORMAL );
	UpdateWindow( hWnd_ );
}

bool Window::is_active()
{
	return (GetFocus() == hWnd_);
}

int Window::get_width() const
{
	return width_;
}

int Window::get_height() const
{
	return height_;
}

HWND Window::get_handle() const
{
	return hWnd_;
}