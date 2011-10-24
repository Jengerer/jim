#include "mouse.h"

Mouse::Mouse( Window* window )
{
	set_window( window );
	poll();
}

void Mouse::set_window( Window* window )
{
	window_ = window;
}

void Mouse::poll( void )
{
	GetCursorPos( &position_ );
	ScreenToClient( window_->get_handle(), &position_ );

	// Clamp X.
	if (position_.x < 0) {
		position_.x = 0;
	}
	else if (position_.x > window_->get_width()) {
		position_.x = window_->get_width();
	}

	// Clamp Y.
	if (position_.y < 0) {
		position_.y = 0;
	}
	else if (position_.y > window_->get_height()) {
		position_.y = window_->get_height();
	}
}

int Mouse::get_x() const
{
	return (int)position_.x;
}

int Mouse::get_y() const
{
	return (int)position_.y;
}

bool Mouse::is_touching( const Component* component ) const
{
	// Get mouse coordinates.
	int x = get_x();
	int y = get_y();

	// Get object bounds.
	int left = static_cast<int>(component->get_x());
	int right = static_cast<int>(left + component->get_width());
	int top = static_cast<int>(component->get_y());
	int bottom = static_cast<int>(top + component->get_height());

	// Check collision.
	return (x >= left) && (x <= right) && (y >= top) && (y <= bottom);
}