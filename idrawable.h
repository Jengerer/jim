#pragma once

#include "directx.h"

class IDrawable
{

public:

	virtual void OnDraw( DirectX* directX ) = 0;

};