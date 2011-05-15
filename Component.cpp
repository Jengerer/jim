#include "Component.h"

Component::Component( float x, float y )
{
	SetPosition( x, y );
	SetAlpha( 255 );
}

Component::~Component()
{
	// Component is destroyed.
}

void Component::SetX( float x )
{
	x_ = x;
}

void Component::SetY( float y )
{
	y_ = y;
}

void Component::SetPosition( float x, float y )
{
	SetX( x );
	SetY( y );
}

int Component::GetWidth() const
{
	return width_;
}

int Component::GetHeight() const
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
	alpha_ = alpha;
	if (alpha_ > 255) {
		alpha_ = 255;
	}
	else if (alpha_ < 0) {
		alpha_ = 0;
	}
}

int Component::GetAlpha( void ) const
{
	return alpha_;
}