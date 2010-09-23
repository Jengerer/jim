#pragma once

#include "Drawable.h"

class Popup: public Drawable
{
public:
	virtual void drawObject(DirectX* pDirectX) = 0;
	virtual void mouseInteract() = 0;
};