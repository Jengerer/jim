#pragma once

#include "Drawable.h"
#include "MouseListener.h"

class Draggable: public Drawable
{
public:
	Draggable( float x = 0.0f, float y = 0.0f );

	virtual void onDrag();
	virtual void onRelease();

	virtual float getX() const;
	virtual float getY() const;

	bool			isDragging;

private:
	MouseListener*	mouseListener_;
	float			offsetX_, offsetY_;
};