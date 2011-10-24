#include "component.h"

Component::Component( float x, float y )
{
	set_position( x, y );
	set_size( 0, 0 );
	set_alpha( 255 );
}

Component::~Component( void )
{
	// Component is destroyed.
}

void Component::set_position( float x, float y )
{
	x_ = x;
	y_ = y;
}

int Component::get_width( void ) const
{
	return width_;
}

int Component::get_height( void ) const
{
	return height_;
}

void Component::set_size( int width, int height )
{
	width_ = width;
	height_ = height;
}

float Component::get_x() const
{
	return x_;
}

float Component::get_y() const
{
	return y_;
}

void Component::set_alpha( int alpha )
{
	if (alpha > COMPONENT_MAX) {
		alpha_ = COMPONENT_MAX;
	}
	else if (alpha < COMPONENT_MIN) {
		alpha_ = COMPONENT_MIN;
	}
	else {
		alpha_ = static_cast<GLubyte>(alpha);
	}
}

GLubyte Component::GetAlpha( void ) const
{
	return alpha_;
}