#pragma once

#include "container.h"
#include "imouse_handler.h"

class Draggable: public Container, public IMouseHandler
{

public:

	Draggable( float x = 0.0f, float y = 0.0f );
	virtual ~Draggable( void );

	virtual void OnDraw( DirectX *directX );

	// Mouse handling.
	virtual bool OnMouseMoved( Mouse *mouse );
	virtual bool OnLeftClicked( Mouse *mouse );
	virtual bool OnLeftReleased( Mouse *mouse );

	bool				HasParent() const;
	const Container*	GetParent() const;
	void				SetParent( const Container* container );

	bool IsDragging( void ) const;
	virtual void OnDrag( const Mouse* mouse );
	virtual void OnRelease( void );

	virtual float GetGlobalX( void ) const;
	virtual float GetGlobalY( void ) const;

private:

	void Initialize( void );
	void SetOffset( float x, float y );
	void SetDragging( bool isDragging );

private:

	const Mouse			*mouse_;
	const Container*	parent_;

	bool			isDragging_;
	float			offsetX_, offsetY_;

};