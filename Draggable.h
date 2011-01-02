#pragma once

#include "Panel.h"

class Draggable: public Panel
{
public:
	Draggable( float x = 0.0f, float y = 0.0f );
	virtual ~Draggable();

	virtual void onDrag( Mouse* mouse, Container* parent );
	virtual void onMove();
	virtual void onRelease();
	bool isDragging() const;

	virtual float getX() const;
	virtual float getY() const;

private:
	Mouse*	mouse_;
	Container* parent_;

	bool			isDragging_;
	float			offsetX_, offsetY_;
};