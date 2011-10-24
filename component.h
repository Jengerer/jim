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
	virtual void	set_position( float x, float y );
	
	// Position getters.
	virtual float	get_x() const;
	virtual float	get_y() const;

	// Visibility functions.
	virtual void	set_alpha( int alpha ); // Using int to handle underflow.
	GLubyte	GetAlpha( void ) const;

	// Size functions.
	virtual void	set_size( int width, int height );
	virtual int		get_width( void ) const;
	virtual int		get_height( void ) const;

private:

	float x_;
	float y_;
	int width_;
	int height_;
	GLubyte alpha_;

};

#endif // COMPONENT_H