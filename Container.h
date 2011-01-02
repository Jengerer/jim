#pragma once

#include <deque>
#include "Component.h"

class Container: public Component
{
public:
	Container( float x = 0.0f, float y = 0.0f );
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