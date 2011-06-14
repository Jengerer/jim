#pragma once

#include "DirectX.h"

class IDrawable
{

public:

	virtual void OnDraw( DirectX* directX ) = 0;

};