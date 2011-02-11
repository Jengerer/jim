#include "Mouse.h"

Mouse::Mouse( Window* window )
{
	leftClicked_ = rightClicked_ = false;
	setWindow( window );

	// Zero position.
	position_.x = position_.y = 0;
}

void Mouse::setWindow( Window* window )
{
	window_ = window;
}

void Mouse::pollPosition()
{
	GetCursorPos( &position_ );
	ScreenToClient( window_->getHandle(), &position_ );

	if (position_.x < 0) {
		position_.x = 0;
	}
	else if (position_.x > window_->getWidth()) {
		position_.x = window_->getWidth();
	}

	if (position_.y < 0) {
		position_.y = 0;
	}
	else if (position_.y > window_->getHeight()) {
		position_.y = window_->getHeight();
	}
}

int Mouse::getX() const
{
	return (int)position_.x;
}

int Mouse::getY() const
{
	return (int)position_.y;
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

void Mouse::setLeftMouse( bool isClicked ) {
	leftClicked_ = isClicked;
}

void Mouse::setRightMouse( bool isClicked ) {
	rightClicked_ = isClicked;
}

bool Mouse::isLeftDown() {
	return leftClicked_;
}

bool Mouse::isRightDown() {
	return rightClicked_;
}