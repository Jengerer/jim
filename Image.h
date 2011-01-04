#pragma once

#include "Component.h"
#include "Drawable.h"

class Image: public Component, public Drawable
{
public:
	Image( float x = 0.0f, float y = 0.0f );
	virtual ~Image();
};