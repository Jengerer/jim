#ifndef DRAGGABLE_H
#define DRAGGABLE_H

#include "constrained_container.h"
#include "imouse_handler.h"

class Draggable: public ConstrainedContainer, public IMouseHandler
{

public:

	Draggable( float x = 0.0f, float y = 0.0f );
	virtual ~Draggable( void );

	virtual void Draw( Graphics2D* graphics );

	// Mouse handling.
	virtual bool on_mouse_moved( Mouse *mouse );
	virtual bool on_mouse_clicked( Mouse *mouse );
	virtual bool on_mouse_released( Mouse *mouse );

	bool				HasParent() const;
	const Container*	GetParent() const;
	void				SetParent( const Container* container );

	bool IsDragging( void ) const;
	virtual void BeginDragging( const Mouse* mouse );
	virtual void EndDragging( void );

	virtual float GetX( void ) const;
	virtual float GetY( void ) const;

private:

	void Initialize( void );
	void SetOffset( float x, float y );
	void SetDragging( bool isDragging );

private:

	const Mouse			*mouse_;
	const Container*	parent_;

	bool				isDragging_;
	float				offsetX_, offsetY_;

};

#endif // DRAGGABLE_H