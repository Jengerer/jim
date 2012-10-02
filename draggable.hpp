#ifndef DRAGGABLE_H
#define DRAGGABLE_H

#include <jui/layout/constrained_container.hpp>
#include <jui/io/mouse_handler_interface.hpp>

class Draggable: public JUI::ConstrainedContainer, public JUI::MouseHandlerInterface
{

public:

	Draggable( float x = 0.0f, float y = 0.0f );
	virtual ~Draggable( void );

	virtual void draw( JUI::Graphics2D* graphics );

	// JUI::Mouse* handling.
	virtual bool on_mouse_moved( JUI::Mouse *mouse );
	virtual bool on_mouse_clicked( JUI::Mouse* mouse );
	virtual bool on_mouse_released( JUI::Mouse* mouse );

	bool				has_parent() const;
	const Container*	get_parent() const;
	void				set_parent( const Container* container );

	bool is_dragging( void ) const;
	virtual void begin_dragging( const JUI::Mouse* mouse );
	virtual void end_dragging( void );

	virtual float get_x( void ) const;
	virtual float get_y( void ) const;

private:

	void Initialize( void );
	void SetOffset( float x, float y );
	void SetDragging( bool isDragging );

private:

	const JUI::Mouse*			*mouse_;
	const Container*	parent_;

	bool				isDragging_;
	float				offsetX_, offsetY_;

};

#endif // DRAGGABLE_H