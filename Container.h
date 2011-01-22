#pragma once

#include <deque>
#include "Component.h"

class Container: public Component
{
public:
	Container();
	Container( float x, float y );
	virtual ~Container();

	// Relative position modifiers.
	virtual void setPosition( float x, float y );
	virtual void updatePosition();

	void add( Component *component );
	void remove( Component *component );
	void trash( Component *component );
	void emptyTrash();

protected:
	deque<Component*> componentStack_;
	deque<Component*> componentTrash_;
};