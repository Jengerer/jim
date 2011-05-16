#pragma once

#include <vector>
#include "IDrawable.h"

using namespace std;

// Prototypes.
class Component: public IDrawable
{

public:

	Component( float x = 0.0f, float y = 0.0f );
	virtual ~Component( void );

	// Position functions.
	virtual void	SetPosition( float x, float y );
	virtual void	SetX( float x );
	virtual void	SetY( float y );
	virtual float	GetX( void ) const;
	virtual float	GetY( void ) const;

	// Visibility functions.
	virtual void	SetAlpha( int alpha );
	int				GetAlpha( void ) const;

	// Size functions.
	virtual int		GetWidth( void ) const;
	virtual int		GetHeight( void ) const;
	virtual void	SetSize( int width, int height );

private:

	float x_, y_;
	int width_, height_;
	int alpha_;

};