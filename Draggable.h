#pragma once

#include "Drawable.h"
#include "MouseListener.h"

class Draggable: public Drawable
{
public:
	Draggable( MouseListener* mouseListener, float x, float y );

	virtual void onDrag() = 0;
	virtual void onRelease() = 0;

	virtual float getX() const;
	virtual float getY() const;

private:
	MouseListener* mouseListener_;
	float offsetX_, offsetY_;
};