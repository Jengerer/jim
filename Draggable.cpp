#include "Draggable.h"

Draggable::Draggable( float x, float y ) : Drawable( x, y )
{
	isDragging = false;
	offsetX_ = offsetY_ = 0.0f;
}

float Draggable::getX() const
{
	//TODO: Make dragging work.
	return this->x;
}

float Draggable::getY() const
{
	//TODO: Make dragging work.
	return this->y;
}

void Draggable::onDrag() {
	// Get movement control.
	isDragging = true;
}

void Draggable::onRelease() {
	// Set this as the permanent position.
	x = getX();
	y = getY();

	// Release control.
	isDragging = false;
}