#ifndef COMPONENT_H
#define COMPONENT_H

#include "idrawable.h"

// Prototypes.
class Component : public IDrawable
{

public:

	Component( float x = 0.0f, float y = 0.0f );
	virtual ~Component( void );

	// Position functions.
	virtual void	SetPosition( float x, float y );
	
	// Position getters.
	virtual float	GetX() const;
	virtual float	GetY() const;

	// Visibility functions.
	virtual void	SetAlpha( int alpha ); // Using int to handle underflow.
	GLubyte	GetAlpha( void ) const;

	// Size functions.
	virtual void	SetSize( int width, int height );
	virtual int		GetWidth( void ) const;
	virtual int		GetHeight( void ) const;

private:

	float x_;
	float y_;
	int width_;
	int height_;
	GLubyte alpha_;

};

#endif // COMPONENT_H