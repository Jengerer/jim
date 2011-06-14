#include "mouse.h"

Mouse::Mouse( Window* window )
{
	SetWindow( window );
	Poll();
}

void Mouse::SetWindow( Window* window )
{
	window_ = window;
}

void Mouse::Poll( void )
{
	GetCursorPos( &position_ );
	ScreenToClient( window_->getHandle(), &position_ );

	// Clamp X.
	if (position_.x < 0) {
		position_.x = 0;
	}
	else if (position_.x > window_->GetWidth()) {
		position_.x = window_->GetWidth();
	}

	// Clamp Y.
	if (position_.y < 0) {
		position_.y = 0;
	}
	else if (position_.y > window_->GetHeight()) {
		position_.y = window_->GetHeight();
	}
}

int Mouse::GetX() const
{
	return (int)position_.x;
}

int Mouse::GetY() const
{
	return (int)position_.y;
}

bool Mouse::IsTouching( const Component* component ) const
{
	// Get mouse coordinates.
	int x = GetX();
	int y = GetY();

	// Get object bounds.
	int left = component->GetGlobalX();
	int right = left + component->GetWidth();
	int top = component->GetGlobalY();
	int bottom = top + component->GetHeight();

	// Check collision.
	return (x >= left) && (x <= right) && (y >= top) && (y <= bottom);
}