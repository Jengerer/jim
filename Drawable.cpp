#include "Drawable.h"

Drawable::Drawable( float x, float y )
{
	// Just set the position.
	setPosition(x, y);
	alpha = 100;
}

void Drawable::setPosition( float x, float y )
{
	setPosition(x, y);
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

int Drawable::getWidth() const
{
	const Texture* texture = getTexture();
	return texture->getWidth();
}

int Drawable::getHeight() const
{
	const Texture* texture = getTexture();
	return texture->getHeight();
}