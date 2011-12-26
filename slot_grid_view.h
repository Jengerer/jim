#ifndef SLOT_GRID_VIEW_H
#define SLOT_GRID_VIEW_H

#include <vector>

#include <jui/grid_layout.h>
#include <jui/mouse.h>

#include "slot_vector.h"
#include "slot_view.h"

class SlotGridView : public GridLayout
{
public:

	SlotGridView( unsigned int width, unsigned int spacing );

	void add_slots( const SlotVector* slots );
	void add_slots( const SlotVector* slots, unsigned int startIndex, unsigned int endIndex );

	SlotView* get_touching_slot( Mouse* mouse ) const;

private:

	void AddSlot( Slot* slot );

private:

	vector<SlotView*> slotViews_;

};

#endif // SLOT_GRID_VIEW_H