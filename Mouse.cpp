#include "Mouse.h"

vector<Component*> Mouse::components_;
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

void Mouse::addComponent( Component* component )
{
	size_t mid = 0;
	size_t min = 0, max = components_.size();
	while (min < max) {
		mid = (min + max) >> 1;

		if (components_[mid] < component) {
			min = mid + 1;
		}
		else {
			max = mid;
		}
	}

	if (components_.empty() || (components_[mid] != component)) {
		components_.insert( components_.begin() + mid, component );
	}
}

void Mouse::removeComponent( Component* component )
{
	// Don't loop over empty set.
	if (components_.empty())
		return;

	size_t mid = 0;
	size_t min = 0, max = components_.size();
	while (min < max) {
		mid = (min + max) >> 1;

		if (components_[mid] < component) {
			min = mid + 1;
		}
		else {
			max = mid;
		}
	}

	if (components_[mid] != component) {
		components_.erase( components_.begin() + mid );
	}
}

void Mouse::clearComponents() {
	// Just clear it.
	components_.clear();
}