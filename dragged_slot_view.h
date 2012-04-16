#ifndef DRAGGED_SLOT_VIEW_H
#define DRAGGED_SLOT_VIEW_H

#include <jui/imouse_handler.h>

#include "slot.h"
#include "slot_view.h"

/*
 * Slot view that can be carried by mouse.
 */
class DraggedSlotView: public SlotView, public IMouseHandler
{

public:

	DraggedSlotView( Slot* slot );
	virtual ~DraggedSlotView( void );

	// Mouse handling.
	bool	on_mouse_clicked( Mouse* mouse );
	bool	on_mouse_released( Mouse* mouse );
	bool	on_mouse_moved( Mouse* mouse );

	// Offset functions.
	void	set_offset( float x, float y );

private:

	float offset_x_;
	float offset_y_;

};

#endif // DRAGGED_SLOT_VIEW_H