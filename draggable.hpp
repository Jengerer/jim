#ifndef DRAGGABLE_H
#define DRAGGABLE_H

#include <jui/layout/constrained_container.hpp>
#include <jui/io/mouse_handler_interface.hpp>

class Draggable: public JUI::ConstrainedContainer, public JUI::MouseHandlerInterface
{

public:

	Draggable( float x = 0.0f, float y = 0.0f );
	virtual ~Draggable( void );

	// Drawable function.
	virtual void draw( JUI::Graphics2D* graphics );

	// Mouse handling.
	virtual JUI::IOResult on_mouse_moved( JUI::Mouse* mouse );
	virtual JUI::IOResult on_mouse_clicked( JUI::Mouse* mouse );
	virtual JUI::IOResult on_mouse_released( JUI::Mouse* mouse );

	// Parent handling.
	bool has_parent( void ) const;
	const Container* get_parent( void ) const;
	void set_parent( const Container* container );

	// Dragging session handling.
	bool is_dragging( void ) const;
	virtual void begin_dragging( const JUI::Mouse* mouse );
	virtual void end_dragging( void );

	// Over-written position getters for dragging case.
	virtual float get_x( void ) const;
	virtual float get_y( void ) const;

private:

	// Dragging offset functions.
	void set_offset( float x, float y );
	void set_dragging( bool is_dragging );

private:

	const JUI::Mouse* mouse_;
	const Container* parent_;

	bool is_dragging_;
	float offset_x_, offset_y_;

};

#endif // DRAGGABLE_H