#include "component.h"

Component::Component( float localX, float localY )
{
	SetLocalPosition( localX, localY );
	SetGlobalPosition( localX, localY );
	SetAlpha( 255 );
}

Component::~Component( void )
{
	// Component is destroyed.
}

void Component::SetGlobalPosition( float globalX, float globalY )
{
	globalX_ = globalX;
	globalY_ = globalY;
}

void Component::SetLocalPosition( float localX, float localY )
{
	localX_ = localX;
	localY_ = localY;
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

float Component::GetGlobalX( void ) const
{
	return globalX_;
}

float Component::GetGlobalY( void ) const
{
	return globalY_;
}

float Component::GetLocalX( void ) const
{
	return localX_;
}

float Component::GetLocalY( void ) const
{
	return localY_;
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