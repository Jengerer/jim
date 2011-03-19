#include "Application.h"

/* Application constructor. */
Application::Application(
	const char* title,
	HINSTANCE instance,
	int width, int height ) : Container( 0.0f, 0.0f )
{
	directX_ = new DirectX(	instance, title, width, height );
	arrow_ = LoadCursor( 0, IDC_ARROW );
	hand_ = LoadCursor( 0, IDC_HAND );

	// Add mouse keys by default.
	listenKey( VK_LBUTTON );
	listenKey( VK_RBUTTON );

	setState( APPLICATION_STATE_START );
}

Application::~Application()
{
	if (directX_ != 0)
	{
		delete directX_;
		directX_ = 0;
	}
}

void Application::exitApplication()
{
	PostMessage( getWindow()->getHandle(), WM_DESTROY, 0, 0 );
}

Window* Application::getWindow() const
{
	return directX_;
}

int Application::getWidth() const
{
	return directX_->getWidth();
}

int Application::getHeight() const
{
	return directX_->getHeight();
}

void Application::setState( EApplicationState state )
{
	state_ = state;
}

EApplicationState Application::getState() const
{
	return state_;
}

void Application::redraw()
{
	// Start redraw.
	if (directX_->beginDraw()) {
		onRedraw();
		directX_->endDraw();
	}
	else {
		throw Exception( "Failed to begin drawing." );
	}
}

void Application::handleMouse()
{
	// Skip if not in focus.
	if (GetFocus() == getWindow()->getHandle()) {
		// Move mouse every frame.
		mouseMoved( mouse_ );

		// Handle mouse events.
		if (isClicked( VK_LBUTTON ))  {
			mouse_->setLeftMouse( true );
			leftClicked( mouse_ );
		}
		else if (isReleased( VK_LBUTTON )) {
			mouse_->setLeftMouse( false );
			leftReleased( mouse_ );
		}
		else if (isClicked( VK_RBUTTON )) {
			mouse_->setRightMouse( true );
			rightClicked( mouse_ );
		}
		else if (isReleased( VK_RBUTTON )) {
			mouse_->setRightMouse( true );
			rightReleased( mouse_ );
		}
	}
}

void Application::onRedraw()
{
	draw( directX_ );
}