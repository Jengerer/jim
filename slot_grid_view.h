#ifndef SLOT_GRID_VIEW_H
#define SLOT_GRID_VIEW_H

#include <vector>

#include <jui/grid_layout.h>
#include <jui/mouse.h>

#include "slot_array.h"
#include "slot_view.h"

/*
 * Visual component for slot grid.
 */
class SlotGridView : public GridLayout
{
public:

	SlotGridView( const SlotArray* slots,
		unsigned int grid_width,
		unsigned int spacing );

	// Mouse handling.
	SlotView* get_touching_slot( Mouse* mouse ) const;

private:

	// Slot management.
	void add_slots( const SlotArray* slots );
	void add_slot( Slot* slot );

private:

	vector<SlotView*> slot_views_;

};

#endif // SLOT_GRID_VIEW_H