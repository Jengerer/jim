#pragma once

#include <deque>
#include "Component.h"

class Container: public Component
{
public:
	Container();
	Container( float x, float y );
	virtual ~Container();

	// Mouse handling event.
	virtual bool mouseEvent( Mouse *mouse, EMouseEvent eventType );

	// Relative position modifiers.
	virtual void setPosition( float x, float y );
	virtual void updatePosition();

	void add( Component *component );
	void remove( Component *component );

protected:
	deque<Component*> componentStack_;
};