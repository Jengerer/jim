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

void Mouse::triggerEvent( Component* component, EMouseEvent eventType )
{
	component->callMouseListener( this, eventType );
}

bool Mouse::isTouching( const Component* component )
{
	int x = getX();
	int y = getY();

	int left = component->getX();
	int right = left + component->getWidth();
	int top = component->getY();
	int bottom = top + component->getHeight();
	return ((x >= left) && (x <= right) && (y >= top) && (y <= bottom));
}