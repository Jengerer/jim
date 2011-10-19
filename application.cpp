#include "application.h"

/* Application constructor. */
Application::Application( int width, int height )
{
	// Uncreated members null.
	graphics_ = nullptr;
	window_ = nullptr;
	mouse_ = nullptr;
	
	// Add mouse keys by default.
	AddKey( VK_LBUTTON );

	SetSize( width, height );
}

Application::~Application()
{
	close_interfaces();
}

void Application::load_interfaces( const char* title, HINSTANCE instance )
{
	// Create window and 2D graphics handle.
	window_ = new Window( instance, title, GetWidth(), GetHeight() );
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

void Application::ExitApplication()
{
	PostMessage( GetWindow()->getHandle(), WM_DESTROY, 0, 0 );
}

Window* Application::GetWindow() const
{
	return window_;
}

void Application::RunApplication( void )
{
	UpdateKeys();
	if ( GetFocus() == GetWindow()->getHandle() ) {
		UpdateMouse();
		HandleKeyboard();
	}
}

void Application::DrawFrame( void )
{
	// Start redraw.
	graphics_->clear_scene();
	Draw( graphics_ );
	graphics_->swap_buffers();
}

void Application::UpdateMouse( void )
{
	// Update mouse regardless of focus.
	mouse_->Poll();
	MouseMoved( mouse_ );

	// Skip if not in focus.
	if ( GetFocus() == GetWindow()->getHandle() ) {
		// Handle mouse events.
		if ( IsKeyClicked( VK_LBUTTON ) )  {
			MouseClicked( mouse_ );
		}
		else if ( IsKeyReleased( VK_LBUTTON ) ) {
			MouseReleased( mouse_ );
		}
	}
}