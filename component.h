#pragma once

#include "idrawable.h"

// Prototypes.
class Component : public IDrawable
{

public:

	Component( float localX = 0.0f, float localY = 0.0f );
	virtual ~Component( void );

	// Position functions.
	virtual void	SetGlobalPosition( float globalX, float globalY );
	virtual void	SetLocalPosition( float localX, float localY );
	
	// Position getters.
	virtual float	GetGlobalX( void ) const;
	virtual float	GetGlobalY( void ) const;
	virtual float	GetLocalX( void ) const;
	virtual float	GetLocalY( void ) const;

	// Visibility functions.
	virtual void	SetAlpha( int alpha );
	int				GetAlpha( void ) const;

	// Size functions.
	virtual int		GetWidth( void ) const;
	virtual int		GetHeight( void ) const;
	virtual void	SetSize( int width, int height );

private:

	float globalX_, globalY_;
	float localX_, localY_;
	int width_, height_;
	int alpha_;

};