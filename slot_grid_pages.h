#ifndef SLOT_GRID_PAGES_H
#define SLOT_GRID_PAGES_H

#include "constrained_container.h"
#include "horizontal_layout.h"
#include "mouse.h"
#include "slot_grid_view.h"
#include "slot_vector.h"

class SlotGridPages : public ConstrainedContainer
{
public:

	SlotGridPages( unsigned int pageWidth, unsigned int pageHeight,
		unsigned int pageSpacing, unsigned int slotSpacing );

	void AddPages( const SlotVector* slots );

	void Pack();

	Slot* GetTouchingSlot( Mouse* mouse ) const;
	SlotGridView* GetCurrentPage() const;

	void NextPage();
	void PreviousPage();
	virtual void UpdateView();

private:

	// Dimension attributes.
	unsigned int			pageWidth_;
	unsigned int			pageHeight_;
	unsigned int			pageSpacing_;
	unsigned int			slotSpacing_;

	unsigned int			page_;
	HorizontalLayout*		pagesLayout_;
	vector<SlotGridView*>	slotGridViews_;

};

#endif // SLOT_GRID_PAGES_H