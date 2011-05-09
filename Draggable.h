#pragma once

#include "Container.h"
#include "IMouseHandler.h"

class Draggable: public Container, public IMouseHandler
{

public:

	Draggable( float x = 0.0f, float y = 0.0f );
	virtual ~Draggable( void );

	// Mouse handling.
	virtual bool OnMouseMoved( Mouse *mouse );
	virtual bool OnLeftClicked( Mouse *mouse );
	virtual bool OnLeftReleased( Mouse *mouse );
	virtual bool OnRightClicked( Mouse *mouse ) { return false; }
	virtual bool OnRightReleased( Mouse *mouse ) { return false; }

	void SetParent( Container *container );
	virtual void OnDrag( Mouse* mouse );
	virtual void OnRelease( void );
	bool IsDragging( void ) const;

	virtual float GetX( void ) const;
	virtual float GetY( void ) const;

private:

	Mouse*			mouse_;
	Container*		parent_;

	bool			isDragging_;
	float			offsetX_, offsetY_;

};