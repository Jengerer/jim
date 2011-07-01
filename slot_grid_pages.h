#ifndef SLOT_GRID_PAGES_H
#define SLOT_GRID_PAGES_H

#include "horizontal_layout.h"
#include "mouse.h"
#include "slot_grid_view.h"
#include "slot_vector.h"

class SlotGridPages : public Container
{
public:

	SlotGridPages( const SlotVector* slots,
		unsigned int width, 
		unsigned int height,
		unsigned int pageSpacing,
		unsigned int slotSpacing );

	void Pack();

	Slot* GetTouchingSlot( Mouse* mouse ) const;
	SlotGridView* GetCurrentPage() const;

	void NextPage();
	void PreviousPage();
	virtual void UpdateView();

private:

	unsigned int			page_;
	HorizontalLayout*		pagesLayout_;
	vector<SlotGridView*>	slotGridViews_;

};

#endif // SLOT_GRID_PAGES_H