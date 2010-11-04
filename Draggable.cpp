#include "Draggable.h"

Draggable::Draggable( float x, float y ) : Panel( x, y )
{
	isDragging = false;
	offsetX_ = offsetY_ = 0.0f;
}

Draggable::~Draggable()
{
	// Draggable destroyed.
}

float Draggable::getX() const
{
	// TODO: Make dragging work.
	return Component::getX();
}

float Draggable::getY() const
{
	// TODO: Make dragging work.
	return Component::getY();
}

void Draggable::onDrag() {
	// Get movement control.
	isDragging = true;
}

void Draggable::onRelease() {
	// Set this as the permanent position.
	setPosition( getX(), getY() );

	// Release control.
	isDragging = false;
}