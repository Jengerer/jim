#include "mouse.h"

/*
 * Mouse constructor.
 */
Mouse::Mouse( Window* window )
{
	set_window( window );
	poll();
}

/*
 * Sets the window for mouse position polling.
 */
void Mouse::set_window( Window* window )
{
	window_ = window;
}

/*
 * Polls the mouse position.
 */
void Mouse::poll()
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

/*
 * Sets the mouse's pressed state.
 */
void Mouse::set_pressed( bool is_pressed )
{
	is_pressed_ = is_pressed;
}

/*
 * Gets the mouse's pressed state.
 */
bool Mouse::is_pressed() const
{
	return is_pressed_;
}

/*
 * Gets the mouse X position.
 */
int Mouse::get_x() const
{
	return (int)position_.x;
}

/*
 * Gets the mouse Y position.
 */
int Mouse::get_y() const
{
	return (int)position_.y;
}

/* 
 * Checks if the mouse is within the component's bounding box.
 */
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