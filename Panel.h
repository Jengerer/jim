#pragma once

#include "Drawable.h"
#include "Container.h"

class Panel: public Container, public Drawable
{
public:
	Panel( float x = 0.0f, float y = 0.0f );
	virtual ~Panel();
};