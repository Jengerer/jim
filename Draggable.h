#pragma once

#include "Panel.h"

class Draggable: public Panel
{
public:
	Draggable( float x = 0.0f, float y = 0.0f );
	virtual ~Draggable();

	// Mouse handling.
	virtual bool mouseMoved( Mouse *mouse );
	virtual bool mouseClicked( Mouse *mouse );
	virtual bool mouseReleased( Mouse *mouse );

	virtual void onDrag( Mouse* mouse );
	virtual void onRelease();
	void setParent( Container *container );
	bool isDragging() const;

	virtual float getX() const;
	virtual float getY() const;

private:
	Mouse*	mouse_;
	Container* parent_;

	bool			isDragging_;
	float			offsetX_, offsetY_;
};