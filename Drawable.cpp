#include "Drawable.h"

Drawable::Drawable( float x, float y )
{
	// Just set the position.
	setPosition(x, y);
	alpha = 100;
}

void Drawable::setPosition( float x, float y )
{
	this->x = x;
	this->y = y;
}

bool Drawable::mouseTouching( MouseListener* mouseListener ) const
{
	// Get mouse position.
	int xMouse, yMouse;
	mouseListener->getPosition( &xMouse, &yMouse );

	// Check collision.
	return (xMouse >= x) &&	(xMouse <= x + getWidth()) &&
		(yMouse >= y) && (yMouse <= y + getHeight());
}