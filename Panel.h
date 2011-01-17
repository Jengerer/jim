#pragma once

#include "Drawable.h"
#include "Container.h"

class Panel: public Container, public Drawable
{
public:
	Panel();
	Panel( float x, float y );
	virtual ~Panel();
};