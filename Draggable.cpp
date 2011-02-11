#include "Draggable.h"

Draggable::Draggable( float x, float y ) : Panel( x, y )
{
	isDragging_ = false;
	setParent( 0 );
	offsetX_ = offsetY_ = 0.0f;
}

Draggable::~Draggable()
{
	// Draggable destroyed.
}

bool Draggable::mouseMoved( Mouse *mouse )
{
	// Just update dragging.
	if (isDragging_) {
		updatePosition();
		return true;
	}

	return false;
}

bool Draggable::mouseClicked( Mouse *mouse )
{
	// Start dragging.
	onDrag( mouse );
	return true;
}

bool Draggable::mouseReleased( Mouse *mouse )
{
	// End dragging.
	onRelease();
	return true;
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

/*
 * Purpose: Starts dragging the object.
 * Precondition: parent is set before dragging.
 */
void Draggable::onDrag( Mouse* mouse )
{
	// Set the offset.
	offsetX_ = mouse->getX() - getX();
	offsetY_ = mouse->getY() - getY();

	// Get movement control.
	mouse_ = mouse;
	isDragging_ = true;
}

void Draggable::setParent( Container *container ) {
	parent_ = container;
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