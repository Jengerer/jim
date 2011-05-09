#pragma once

#include "Component.h"

// TODO: Convert to texture and simple draw. This is all this should be.
class Image: public Component
{
public:
	Image( float x = 0.0f, float y = 0.0f );
	virtual ~Image();
};