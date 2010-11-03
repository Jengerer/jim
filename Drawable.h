#pragma once

#include "Component.h"
#include "DirectX.h"

class Drawable: public Component
{
public:
	Drawable( float x = 0.0f, float y = 0.0f );
	virtual ~Drawable();

	// Pure virtual; override me.
	virtual void draw( DirectX* directX ) = 0;

	// Drawing parameters.
	int		alpha;
};