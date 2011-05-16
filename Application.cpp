#include "Application.h"

/* Application constructor. */
Application::Application( int width, int height )
{
	// Uncreated members null.
	directX_ = nullptr;
	mouse_ = nullptr;
	arrow_ = nullptr;
	hand_ = nullptr;

	// Add mouse keys by default.
	AddKey( VK_LBUTTON );
	AddKey( VK_RBUTTON );

	// We're just beginning, so much to do!
	SetState( APPLICATION_STATE_START );
	SetSize( width, height );
}

Application::~Application()
{
	CloseInterfaces();
}

void Application::LoadInterfaces( const char* title, HINSTANCE instance )
{
	// Get DirectX running.
	directX_ = new DirectX(	instance, title, GetWidth(), GetHeight() );
	directX_->LoadInterfaces();

	// Create mouse objects and cursors.
	mouse_ = new Mouse( directX_ );
	arrow_ = LoadCursor( nullptr, IDC_ARROW );
	hand_ = LoadCursor( nullptr, IDC_HAND );
}

void Application::CloseInterfaces( void )
{
	if (directX_ != nullptr) {
		delete directX_;
		directX_ = nullptr;
	}
}

void Application::ExitApplication()
{
	PostMessage( GetWindow()->getHandle(), WM_DESTROY, 0, 0 );
}

Window* Application::GetWindow() const
{
	return directX_;
}

void Application::SetState( EApplicationState state )
{
	state_ = state;
}

EApplicationState Application::GetState( void ) const
{
	return state_;
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
	if ( directX_->BeginDraw() ) {
		OnDraw( directX_ );
		directX_->EndDraw();
	}
	else {
		throw Exception( "Failed to call Direct3D BeginDraw." );
	}
}

void Application::UpdateMouse( void )
{
	// Update mouse regardless of focus.
	mouse_->Poll();
	OnMouseMoved( mouse_ );

	// Skip if not in focus.
	if ( GetFocus() == GetWindow()->getHandle() ) {
		// Handle mouse events.
		if ( IsKeyClicked( VK_LBUTTON ) )  {
			OnLeftClicked( mouse_ );
		}
		else if ( IsKeyReleased( VK_LBUTTON ) ) {
			OnLeftReleased( mouse_ );
		}
		else if ( IsKeyClicked( VK_RBUTTON ) ) {
			OnRightClicked( mouse_ );
		}
		else if ( IsKeyReleased( VK_RBUTTON ) ) {
			OnRightReleased( mouse_ );
		}
	}
}