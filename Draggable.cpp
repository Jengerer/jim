#include "Draggable.h"

Draggable::Draggable( float x, float y ) : Panel( x, y )
{
	isDragging_ = false;
	offsetX_ = offsetY_ = 0.0f;
}

Draggable::~Draggable()
{
	// Draggable destroyed.
}

float Draggable::getX() const
{
	if (isDragging_) {
		float x = mouse_->getX() - offsetX_;
		if (x < 0) {
			x = 0;
		}
		else if (x + getWidth() > parent_->getWidth()) {
			x = parent_->getWidth() - getWidth();
		}

		return x;
	}

	return Component::getX();
}

float Draggable::getY() const
{
	if (isDragging_) {
		float y = mouse_->getY() - offsetY_;
		if (y < 0) {
			y = 0;
		}
		else if (y + getHeight() > parent_->getHeight()) {
			y = parent_->getHeight() - getHeight();
		}

		return y;
	}

	return Component::getY();
}

void Draggable::onDrag( Mouse* mouse, Container* parent )
{
	// Set the offset.
	offsetX_ = mouse->getX() - getX();
	offsetY_ = mouse->getY() - getY();

	// Get movement control.
	mouse_ = mouse;
	parent_ = parent;
	isDragging_ = true;
}

void Draggable::onMove()
{
	// Update position of children.
	updatePosition();
}

void Draggable::onRelease()
{
	// Save new location.
	setPosition( getX(), getY() );

	// Release control.
	isDragging_ = false;
}

bool Draggable::isDragging() const
{
	return isDragging_;
}