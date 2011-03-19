#pragma once

#include <deque>
#include "Component.h"

class Component;
class Container: public Component
{
public:
	Container();
	Container( float x, float y );
	virtual ~Container();

	// Drawable functions.
	virtual void draw( DirectX *directX );

	// Relative position modifiers.
	virtual void	setPosition( float x, float y );
	virtual void	updatePosition();
	virtual bool	withinBounds( Component *component );
	void			clampChild( Component *component, int padding = 0 );

	void add( Component *component );
	void addBottom( Component *component );
	void remove( Component *component );

protected:
	deque<Component*> componentStack_;
};