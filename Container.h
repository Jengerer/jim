#pragma once

#include <deque>
#include "Component.h"

class Component;
class Container: public Component
{
public:
	Container( float x = 0.0f, float y = 0.0f );
	virtual ~Container();

	// Container resource handling.
	void add( Component *component );
	void addBottom( Component *component );
	void remove( Component *component );
	void removeAll();

	// Children function.
	deque<Component*>* getChildren();

	// Drawable functions.
	virtual void draw( DirectX *directX );

	// Relative position modifiers.
	virtual void	setPosition( float x, float y );
	virtual void	updatePosition();
	virtual bool	withinBounds( Component *component );
	void			clampChild( Component *component, int padding = 0 );

protected:
	deque<Component*> componentStack_;
};