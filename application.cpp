#include "application.h"

#include <tlhelp32.h>

/*
 * Application constructor.
 */
Application::Application( HINSTANCE instance )
{
	// Create default window.
	window_ = new Window( instance );
	
	// Create mouse.
	mouse_ = new Mouse( window_ );

	// Create graphics 2D.
	graphics_ = new Graphics2D( window_ );
}

/*
 * Application destructor.
 */
Application::~Application()
{
	close_interfaces();
}

/*
 * Initialize application interfaces.
 */
void Application::load_interfaces( void )
{
	// Set window size and create.
	window_->set_size( get_width(), get_height() );
	window_->create_window();

	// Initialize graphics.
	graphics_->initialize();
}

/*
 * Clean application interfaces.
 */
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

/*
 * Post the window destroy message.
 */
void Application::exit_application()
{
	PostMessage( get_window()->get_handle(), WM_DESTROY, 0, 0 );
}

/*
 * Return the number of processes with the given name.
 */
unsigned int Application::get_process_count( const char* process_name )
{
	unsigned int count = 0;

	// Find all processes with name process_name.
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof( PROCESSENTRY32 );
	HANDLE snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if (Process32First( snapshot, &entry )) {
		// Search for process name.
		while (Process32Next( snapshot, &entry )) {
			if (strcmp( entry.szExeFile, process_name ) == 0) {
				++count;
			}
		}
	}

	return count;
}

/*
 * Get the application's window.
 */
Window* Application::get_window() const
{
	return window_;
}

void Application::run( void )
{
	// Do nothing.
	trigger_mouse_moved();
}

/*
 * Run the application's draw routine.
 */
void Application::draw_frame( void )
{
	graphics_->clear_scene();
	draw( graphics_ );
	graphics_->swap_buffers();
}

/*
 * Update's the mouse's position and state.
 */
void Application::trigger_mouse_events( void )
{
	// Update mouse position.
	trigger_mouse_moved();

	// Trigger click events on change state.
	bool mouse_pressed = GetAsyncKeyState( VK_LBUTTON ) & 0x8000 != 0;
	if (mouse_pressed != mouse_->is_pressed()) {
		if (mouse_pressed) {
			trigger_mouse_clicked();
		}
		else {
			trigger_mouse_released();
		}
	}
}

/*
 * Updates the mouse position and triggers event.
 */
void Application::trigger_mouse_moved( void )
{
	// Update mouse regardless of focus.
	mouse_->poll();
	on_mouse_moved( mouse_ );
}

/*
 * Triggers mouse click.
 */
void Application::trigger_mouse_clicked( void )
{
	mouse_->set_pressed( true );
	on_mouse_clicked( mouse_ );
}

/*
 * Triggers mouse released.
 */
void Application::trigger_mouse_released( void )
{
	mouse_->set_pressed( false );
	on_mouse_released( mouse_ );
}

/*
 * Update key pressed state and trigger event.
 */
void Application::trigger_key_pressed( int key )
{
	// Update state.
	keyboard_.set_key_state( key, true );
	on_key_pressed( key );
}

/*
 * Update key released state and trigger event.
 */
void Application::trigger_key_released( int key )
{
	// Update state.
	keyboard_.set_key_state( key, false );
	on_key_released( key );
}