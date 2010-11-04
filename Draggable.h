#pragma once

#include "Panel.h"

class Draggable: public Panel
{
public:
	Draggable( float x = 0.0f, float y = 0.0f );
	virtual ~Draggable();

	virtual void onDrag();
	virtual void onRelease();

	virtual float getX() const;
	virtual float getY() const;

	bool			isDragging;

private:
	float			offsetX_, offsetY_;
};