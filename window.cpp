#include "window.h"

// Prototype for window handling.
LRESULT CALLBACK wnd_proc( HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam );

// Default window parameters.
const char* DEFAULT_TITLE = "Default Application";
const int DEFAULT_WIDTH = 640;
const int DEFAULT_HEIGHT = 480;
const bool DEFAULT_FULLSCREEN = false;
const bool DEFAULT_BORDER = true;

/*
 * Window constructor.
 */
Window::Window( HINSTANCE instance )
{
	// Null window handle.
	set_handle( nullptr );

	// Set default parameters.
	set_title( DEFAULT_TITLE );
	set_instance( instance );
	set_size( DEFAULT_WIDTH, DEFAULT_HEIGHT );
	set_fullscreen( DEFAULT_FULLSCREEN );
	set_border( DEFAULT_BORDER );
}

/*
 * Window destructor.
 */
Window::~Window()
{
	// Window has been destroyed.
}

/*
 * Registers and creates the window.
 */
void Window::create_window()
{
	// Check for repeated creations.
	if (get_handle() != nullptr) {
		throw std::runtime_error( "Cannot create window twice." );
	}

	// Register window class.
	if (!register_class()) {
		throw std::runtime_error( "Failed to register class." );
	}

	// Get screen resolution to center window.
	RECT screen;
	GetWindowRect( GetDesktopWindow(), &screen );

	// Adjust bounds based on style.
	RECT windowRect;
	windowRect.left	= 0;
	windowRect.top = 0;
	windowRect.right = get_width();
	windowRect.bottom = get_height();

	// Adjust size for style.
	DWORD display_style = (is_fullscreen() || !has_border() ? WS_POPUP : WS_CAPTION | WS_SYSMENU);
	if (!AdjustWindowRect( &windowRect, display_style, false )) {
		throw std::runtime_error( "Failed to adjust window rectangle." );
	}

	// Create window.
	HWND wnd = CreateWindow(
		title_, title_,
		display_style | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		CW_USEDEFAULT, CW_USEDEFAULT,
		windowRect.right - windowRect.left, 
		windowRect.bottom - windowRect.top,
		NULL, NULL,
		get_instance(),
		NULL );
	if (wnd == nullptr) {
		throw std::runtime_error( "Failed to create window." );
	}

	// Display window.
	ShowWindow( wnd, SW_NORMAL );
	UpdateWindow( wnd );

	// Set window handle.
	set_handle( wnd );
}

/*
 * Sets the window handle.
 */
HWND Window::get_handle() const
{
	return wnd_;
}

/*
 * Gets the window title.
 */
const char* Window::get_title() const
{
	return title_;
}

/*
 * Sets the window title.
 */
void Window::set_title( const char* title )
{
	title_ = title;
}

/*
 * Sets the window size.
 */
void Window::set_size( int width, int height )
{
	width_ = width;
	height_ = height;
}

/*
 * Sets the window fullscreen mode.
 */
void Window::set_fullscreen( bool is_fullscreen )
{
	is_fullscreen_ = is_fullscreen;
}

/*
 * Sets the window border mode.
 */
void Window::set_border( bool has_border )
{
	has_border_ = has_border;
}

/*
 * Gets the width of the window.
 */
int Window::get_width() const
{
	return width_;
}

/*
 * Gets the height of the window.
 */
int Window::get_height() const
{
	return height_;
}

/*
 * Checks if the window is in fullscreen mode.
 */
bool Window::is_fullscreen() const
{
	return is_fullscreen_;
}

/*
 * Checks if the window has a border.
 */
bool Window::has_border() const
{
	return has_border_;
}

/*
 * Returns if the window is currently in user focus.
 */
bool Window::is_active()
{
	return GetFocus() == get_handle();
}

/*
 * Register the window class.
 */
bool Window::register_class()
{
	WNDCLASS wnd_class;
	wnd_class.hInstance		= get_instance();
	wnd_class.hIcon			= NULL;
	wnd_class.hCursor		= LoadCursor( NULL, IDC_ARROW );
	wnd_class.hbrBackground	= NULL;
	wnd_class.cbWndExtra	= 0;
	wnd_class.cbClsExtra	= 0;
	wnd_class.lpfnWndProc	= wnd_proc;
	wnd_class.lpszClassName	= title_;
	wnd_class.lpszMenuName	= NULL;
	wnd_class.style			= CS_OWNDC;

	return RegisterClass( &wnd_class ) != 0;
}

void Window::set_handle( HWND hwnd )
{
	wnd_ = hwnd;
}

void Window::set_instance( HINSTANCE instance )
{
	instance_ = instance;
}

HINSTANCE Window::get_instance() const
{
	return instance_;
}