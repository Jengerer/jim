#pragma once

#include "directx.h"

class IDrawable
{

public:

	virtual void Draw( DirectX* directX ) = 0;

};