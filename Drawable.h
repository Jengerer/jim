#pragma once

#include "DirectX.h"

class Drawable
{
public:
	// Pure virtual; override me.
	virtual void draw( DirectX* directX ) = 0;
};