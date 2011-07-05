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

void Component::SetAlpha( unsigned int alpha )
{
	if (alpha > 255) {
		alpha_ = 255;
	}
	else if (alpha < 0) {
		alpha_ = 0;
	}
	else {
		alpha_ = alpha;
	}
}

unsigned int Component::GetAlpha( void ) const
{
	return alpha_;
}