#include "Window.h"

Window::Window(HINSTANCE hInstance,
			   WNDPROC wndProc,
			   const char* title,
			   int width, int height)
{
	title_ = title;
	width_ = width;
	height_ = height;

	if (!registerClass( hInstance, wndProc )) 
		throw Exception( "Failed to register class." );

	createWindow(hInstance);
}

Window::~Window()
{
	//Window has been destroyed.
}

bool Window::registerClass(HINSTANCE hInstance, WNDPROC wndProc)
{
	WNDCLASSEX wndCls;

	wndCls.hInstance		= hInstance;
	wndCls.hIcon			= NULL;
	wndCls.hIconSm			= NULL;
	wndCls.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wndCls.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);

	wndCls.cbSize			= sizeof(WNDCLASSEX);
	wndCls.cbWndExtra		= 0;
	wndCls.cbClsExtra		= 0;

	wndCls.lpfnWndProc		= wndProc;
	wndCls.lpszClassName	= title_;
	wndCls.lpszMenuName		= NULL;

	wndCls.style			= NULL;

	return RegisterClassEx( &wndCls );
}

void Window::createWindow(HINSTANCE hInstance)
{
	// Get screen resolution to center window.
	RECT screenRect;
	GetWindowRect( GetDesktopWindow(), &screenRect );

	int x = (screenRect.right / 2) - (getWidth() / 2);
	int y = (screenRect.bottom / 2) - (getHeight() / 2);

	// Adjust bounds based on style.
	RECT windowRect;
	windowRect.left	= 0;
	windowRect.top	= 0;
	windowRect.right	= getWidth();
	windowRect.bottom	= getHeight();

	// Create window.
	hWnd_ = CreateWindow(
		title_, title_,
		WS_VISIBLE | WS_POPUP | WS_OVERLAPPED,
		x, y,
		windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
		NULL, NULL,
		hInstance,
		NULL );

	// Add rounded corners.
	HRGN windowRegion = CreateRoundRectRgn(
		windowRect.left,
		windowRect.top,
		windowRect.right,
		windowRect.bottom,
		5, 5 );
	SetWindowRgn( hWnd_, windowRegion, true );
	DeleteObject( windowRegion );

	if (!hWnd_)
		throw Exception( "Failed to create window." );

	ShowWindow( hWnd_, SW_SHOWDEFAULT );
	UpdateWindow( hWnd_ );
}

int Window::getWidth() const
{
	return width_;
}

int Window::getHeight() const
{
	return height_;
}

HWND Window::getHandle() const
{
	return hWnd_;
}