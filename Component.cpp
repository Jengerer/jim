#include "Component.h"

Component::Component( float x, float y )
{
	// Set defaults.
	setPosition( x, y );
}

Component::~Component()
{
	// Component is destroyed.
}

bool Component::leftClicked( Mouse *mouse )
{
	// Default handling.
	return false;
}

bool Component::leftReleased( Mouse *mouse )
{
	// Default handling.
	return false;
}

bool Component::rightClicked( Mouse *mouse )
{
	// Default handling.
	return false;
}

bool Component::rightReleased( Mouse *mouse )
{
	// Default handling.
	return false;
}

bool Component::mouseMoved( Mouse *mouse )
{
	// Default handling.
	return false;
}

void Component::setX( float x ) {
	x_ = x;
}

void Component::setY( float y ) {
	y_ = y;
}

void Component::setPosition( float x, float y ) {
	setX( x );
	setY( y );
}

int Component::getWidth() const {
	return width_;
}

int Component::getHeight() const {
	return height_;
}

void Component::setSize( int width, int height ) {
	width_ = width;
	height_ = height;
}

float Component::getX() const {
	return x_;
}

float Component::getY() const {
	return y_;
}