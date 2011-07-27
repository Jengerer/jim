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
	virtual void	Add( Component *component );
	virtual void	Remove( Component *component );

	// Drawing functions.
	virtual void	Draw( Graphics2D* graphics );

	// Local/global child position handling.
	void			ClampChild( Component *child, float padding = 0.0f ) const;
	virtual bool	IsVisible( Component* child ) const;
	virtual bool	WithinBounds( Component* child ) const;

protected:

	std::vector<Component*>	components_;

};

#endif // CONTAINER_H