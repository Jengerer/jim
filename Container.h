#pragma once

#include <vector>
#include "Component.h"

class Container: public Component
{
public:
	Container( float x = 0.0f, float y = 0.0f );
	virtual ~Container();

	// Relative position modifiers.
	virtual void updatePosition();

	void add( Component *component );
	void remove( Component* component );

protected:
	vector<Component*> components_;
};