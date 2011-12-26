#ifndef DRAGGED_SLOT_VIEW_H
#define DRAGGED_SLOT_VIEW_H

#include <jui/imouse_handler.h>

#include "slot.h"
#include "slot_view.h"

class DraggedSlotView: public SlotView, public IMouseHandler
{

public:

	DraggedSlotView( Slot* slot );
	virtual ~DraggedSlotView();

	bool on_mouse_clicked( Mouse* mouse );
	bool on_mouse_released( Mouse* mouse );
	bool on_mouse_moved( Mouse* mouse );

	void DragSlot( Slot* slot );
	Item* ReleaseSlot();
	void SetOffset( float offsetX, float offsetY );

private:

	float offsetX_;
	float offsetY_;

};

#endif // DRAGGED_SLOT_VIEW_H