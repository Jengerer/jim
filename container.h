#ifndef CONTAINER_H
#define CONTAINER_H

#include <vector>
#include "component.h"

class Container : public Component
{

public:

	Container( float x = 0.0f, float y = 0.0f );
	virtual ~Container();

	// Container resource handling.
	virtual void	add( Component *component );
	virtual void	remove( Component *component );

	// Drawing functions.
	virtual void	draw( Graphics2D* graphics );
	virtual void	set_alpha( int alpha );

	// Local/global child position handling.
	void			clamp_child( Component *child, float padding = 0.0f ) const;
	virtual bool	IsVisible( Component* child ) const;
	virtual bool	WithinBounds( Component* child ) const;

protected:

	std::vector<Component*>	components_;

};

#endif // CONTAINER_H