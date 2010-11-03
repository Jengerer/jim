#include "Mouse.h"

Mouse::Mouse( Window* window )
{
	setWindow( window );
}

void Mouse::setWindow( Window* window )
{
	window_ = window;
}

void Mouse::pollPosition()
{
	GetCursorPos( &position_ );
	ScreenToClient( window_->getHandle(), &position_ );
}

int Mouse::getX() const
{
	return (int)position_.x;
}

int Mouse::getY() const
{
	return (int)position_.y;
}