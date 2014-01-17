#ifndef SLOT_STACK_VIEW_HPP
#define SLOT_STACK_VIEW_HPP

#include <jui/io/mouse_handler_interface.hpp>
#include "slot_view.hpp"
#include "slot_array_interface.hpp"
#include "slot_array_listener.hpp"
#include "slot_array_view_listener.hpp"

/*
 * Single slot view for dragging multiple items in a selection stack.
 */
class SlotStackView
    : public SlotView,
      public SlotArrayListener,
	  public JUI::MouseHandlerInterface
{

public:

    SlotStackView( SlotArrayInterface* slot_array );
    virtual ~SlotStackView( void );

    // Listener functions/interfaces.
	void set_listener( SlotArrayViewListener* listener );
    virtual void on_slot_updated( unsigned int index, const Slot* slot );

	// Item dragging handling.
	void set_drag_offset( int x, int y );
	void begin_dragging( void );
	void end_dragging( void );
	bool is_dragging( void ) const;

	// Mouse movement handling.
	virtual JUI::IOResult on_mouse_moved( JUI::Mouse* mouse );
	virtual JUI::IOResult on_mouse_clicked( JUI::Mouse* mouse );
	virtual JUI::IOResult on_mouse_released( JUI::Mouse* mouse );

private:

	// Dragging position restoring.
	void save_position( void );
	void restore_position( void );

private:

    SlotArrayInterface* slot_array_;
	SlotArrayViewListener* listener_;

	// Dragging members.
	bool is_dragging_;
	int offset_x_;
	int offset_y_;

	// Position to return to after dragging done.
	int return_x_;
	int return_y_;

};

#endif // SLOT_STACK_VIEW_HPP