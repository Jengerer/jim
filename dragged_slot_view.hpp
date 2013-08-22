#ifndef DRAGGED_SLOT_VIEW_H
#define DRAGGED_SLOT_VIEW_H

#include <jui/io/mouse_handler_interface.hpp>

#include "slot.hpp"
#include "slot_view.hpp"

/*
 * Slot view that can be carried by mouse.
 */
class DraggedSlotView: public SlotView, public JUI::MouseHandlerInterface
{

public:

	DraggedSlotView( void );
	virtual ~DraggedSlotView( void );

	void update_alpha( void );

	// JUI::Mouse* handling.
	JUI::IOResult on_mouse_clicked( JUI::Mouse* mouse );
	JUI::IOResult on_mouse_released( JUI::Mouse* mouse );
	JUI::IOResult on_mouse_moved( JUI::Mouse* mouse );

	// Offset functions.
	void set_offset( int x, int y );

private:

	int offset_x_;
	int offset_y_;

};

#endif // DRAGGED_SLOT_VIEW_H