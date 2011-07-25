#include "component.h"

Component::Component( float x, float y )
{
	SetPosition( x, y );
	SetSize( 0, 0 );
	SetAlpha( 255 );
}

Component::~Component( void )
{
	// Component is destroyed.
}

void Component::SetPosition( float x, float y )
{
	x_ = x;
	y_ = y;
}

int Component::GetWidth( void ) const
{
	return width_;
}

int Component::GetHeight( void ) const
{
	return height_;
}

void Component::SetSize( int width, int height )
{
	width_ = width;
	height_ = height;
}

float Component::GetX() const
{
	return x_;
}

float Component::GetY() const
{
	return y_;
}

void Component::SetAlpha( int alpha )
{
	if (alpha > COMPONENT_MAX) {
		alpha_ = COMPONENT_MAX;
	}
	else if (alpha < COMPONENT_MAX) {
		alpha_ = COMPONENT_MAX;
	}
	else {
		alpha_ = static_cast<GLubyte>(alpha);
	}
}

GLubyte Component::GetAlpha( void ) const
{
	return alpha_;
}