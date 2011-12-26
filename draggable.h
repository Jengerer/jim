#ifndef DRAGGABLE_H
#define DRAGGABLE_H

#include <jui/constrained_container.h>
#include <jui/imouse_handler.h>

class Draggable: public ConstrainedContainer, public IMouseHandler
{

public:

	Draggable( float x = 0.0f, float y = 0.0f );
	virtual ~Draggable( void );

	virtual void draw( Graphics2D* graphics );

	// Mouse handling.
	virtual bool on_mouse_moved( Mouse *mouse );
	virtual bool on_mouse_clicked( Mouse *mouse );
	virtual bool on_mouse_released( Mouse *mouse );

	bool				has_parent() const;
	const Container*	get_parent() const;
	void				set_parent( const Container* container );

	bool is_dragging( void ) const;
	virtual void begin_dragging( const Mouse* mouse );
	virtual void end_dragging( void );

	virtual float get_x( void ) const;
	virtual float get_y( void ) const;

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