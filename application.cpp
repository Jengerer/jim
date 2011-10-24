#include "application.h"

#include <tlhelp32.h>

/* Application constructor. */
Application::Application( int width, int height )
{
	// Uncreated members null.
	graphics_ = nullptr;
	window_ = nullptr;
	mouse_ = nullptr;
	
	// Add mouse keys by default.
	AddKey( VK_LBUTTON );

	set_size( width, height );
}

Application::~Application()
{
	close_interfaces();
}

void Application::load_interfaces( const char* title, HINSTANCE instance )
{
	// Create window and 2D graphics handle.
	window_ = new Window( instance, title, get_width(), get_height() );
	graphics_ = new Graphics2D( window_ );
	graphics_->initialize();

	// Create mouse.
	mouse_ = new Mouse( window_ );
}

void Application::close_interfaces( void )
{
	if (graphics_ != nullptr) {
		delete graphics_;
		graphics_ = nullptr;
	}

	if (window_ != nullptr) {
		delete window_;
		window_ = nullptr;
	}
}

void Application::exit_application()
{
	PostMessage( get_window()->get_handle(), WM_DESTROY, 0, 0 );
}

unsigned int Application::get_process_count( const char* process_name )
{
	unsigned int count = 0;

	// Find all processes with name process_name.
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof( PROCESSENTRY32 );
	HANDLE snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if (Process32First( snapshot, &entry )) {
		while (Process32Next( snapshot, &entry )) {
			if (strcmp( entry.szExeFile, process_name ) == 0) {
				++count;
			}
		}
	}

	return count;
}

Window* Application::get_window() const
{
	return window_;
}

void Application::run( void )
{
	UpdateKeys();
	if ( GetFocus() == get_window()->get_handle() ) {
		update_mouse();
		handle_keyboard();
	}
}

void Application::draw_frame( void )
{
	// Start redraw.
	graphics_->clear_scene();
	draw( graphics_ );
	graphics_->swap_buffers();
}

void Application::update_mouse( void )
{
	// Update mouse regardless of focus.
	mouse_->poll();
	on_mouse_moved( mouse_ );

	// Skip if not in focus.
	if ( GetFocus() == get_window()->get_handle() ) {
		// Handle mouse events.
		if ( IsKeyClicked( VK_LBUTTON ) )  {
			on_mouse_clicked( mouse_ );
		}
		else if ( IsKeyReleased( VK_LBUTTON ) ) {
			on_mouse_released( mouse_ );
		}
	}
}