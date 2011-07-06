#ifndef SLOT_GRID_VIEW_H
#define SLOT_GRID_VIEW_H

#include <vector>

#include "slot_vector.h"
#include "slot_view.h"
#include "grid_layout.h"
#include "mouse.h"

class SlotGridView : public GridLayout
{
public:

	SlotGridView( unsigned int width, unsigned int spacing );

	void AddSlots( const SlotVector* slots );
	void AddSlots( const SlotVector* slots, unsigned int startIndex, unsigned int endIndex );

	SlotView* GetTouchingSlot( Mouse* mouse ) const;

private:

	void AddSlot( Slot* slot );

private:

	vector<SlotView*> slotViews_;

};

#endif // SLOT_GRID_VIEW_H