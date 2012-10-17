#ifndef DRAGGABLE_H
#define DRAGGABLE_H

#include <jui/layout/constrained_container.hpp>
#include <jui/io/mouse_handler_interface.hpp>

class Draggable: public JUI::ConstrainedContainer, public JUI::MouseHandlerInterface
{

public:

    Draggable( void );
	Draggable( int x, int y );
	virtual ~Draggable( void );

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

private:

	// Dragging offset functions.
	void set_offset( int x, int y );
	void set_dragging( bool is_dragging );

private:

    // Dragging dependent objects.
	const JUI::Mouse* mouse_;
	const Container* parent_;

    // Dragging state variables.
	bool is_dragging_;
	int offset_x_;
    int offset_y_;

};

#endif // DRAGGABLE_H