#ifndef SLOT_STACK_VIEW_HPP
#define SLOT_STACK_VIEW_HPP

#include <jui/io/mouse_handler_interface.hpp>
#include "slot_view.hpp"
#include "slot_array_interface.hpp"
#include "slot_array_listener.hpp"

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

    // Array listener interface.
    virtual bool on_slot_updated( unsigned int index, const Slot* slot );

	// Item dragging handling.
	void set_drag_offset( int x, int y );
	void set_enabled( bool is_enabled );
	bool is_enabled( void ) const;
	void update_alpha( void );

	// Mouse movement handling.
	virtual JUI::IOResult on_mouse_moved( JUI::Mouse* mouse );
	virtual JUI::IOResult on_mouse_clicked( JUI::Mouse* mouse );
	virtual JUI::IOResult on_mouse_released( JUI::Mouse* mouse );

private:

    SlotArrayInterface* slot_array_;

	// Dragging members.
	bool is_enabled_;
	int offset_x_;
	int offset_y_;

};

#endif // SLOT_STACK_VIEW_HPP