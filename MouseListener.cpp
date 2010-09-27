#include "MouseListener.h"

MouseListener::MouseListener( Window* window )
{
	setWindow( window );
}

void MouseListener::setWindow( Window* window )
{
	window_ = window;
}

void MouseListener::pollMouse()
{
	GetCursorPos( &position_ );
	ScreenToClient( window_->getHandle(), &position_ );
}

void MouseListener::getPosition( int* x, int* y ) const
{
	*x = position_.x;
	*y = position_.y;
}

int MouseListener::getMouseX() const
{
	return (int)position_.x;
}

int MouseListener::getMouseY() const
{
	return (int)position_.y;
}