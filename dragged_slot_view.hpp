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

	DraggedSlotView( Slot* slot );
	virtual ~DraggedSlotView( void );

	// JUI::Mouse* handling.
	bool	on_mouse_clicked( JUI::Mouse* mouse );
	bool	on_mouse_released( JUI::Mouse* mouse );
	bool	on_mouse_moved( JUI::Mouse* mouse );

	// Offset functions.
	void	set_offset( float x, float y );

private:

	float offset_x_;
	float offset_y_;

};

#endif // DRAGGED_SLOT_VIEW_H