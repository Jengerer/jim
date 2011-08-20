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

	if (!registerClass( hInstance )) {
		throw Exception( "Failed to register class." );
	}

	createWindow( hInstance );
}

Window::~Window()
{
	//Window has been destroyed.
}

bool Window::registerClass( HINSTANCE hInstance )
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

void Window::createWindow(HINSTANCE hInstance)
{
	// Get screen resolution to center window.
	RECT screenRect;
	GetWindowRect( GetDesktopWindow(), &screenRect );

	int x = (screenRect.right - GetWidth()) / 2;
	int y = (screenRect.bottom - GetHeight()) / 2;

	// Adjust bounds based on style.
	RECT windowRect;
	windowRect.left	= 0;
	windowRect.top = 0;
	windowRect.right = GetWidth();
	windowRect.bottom = GetHeight();

	// Adjust size for style.
	DWORD displayStyle = WS_CAPTION | WS_SYSMENU;
	if (!AdjustWindowRect( &windowRect, displayStyle, false )) {
		throw Exception( "Failed to adjust window rectangle." );
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
		throw Exception( "Failed to create window." );
	}

	ShowWindow( hWnd_, SW_NORMAL );
	UpdateWindow( hWnd_ );
}

bool Window::isActive()
{
	return (GetFocus() == hWnd_);
}

int Window::GetWidth() const
{
	return width_;
}

int Window::GetHeight() const
{
	return height_;
}

HWND Window::getHandle() const
{
	return hWnd_;
}