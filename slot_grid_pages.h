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

	SlotView* GetTouchingSlot( Mouse* mouse ) const;
	SlotGridView* GetCurrentPage() const;

	unsigned int GetPageIndex() const;
	unsigned int GetPageCount() const;

	bool NextPage();
	bool PreviousPage();
	void UpdateOffset();
	virtual void UpdateView();

protected:

	HorizontalLayout*		pagesLayout_;
	Constraint*				pagesConstraint_;
	float					viewOffset_;

private:

	// Dimension attributes.
	unsigned int			pageWidth_;
	unsigned int			pageHeight_;
	unsigned int			pageSpacing_;
	unsigned int			slotSpacing_;

	// View offset.
	unsigned int			page_;
	vector<SlotGridView*>	slotGridViews_;

};

#endif // SLOT_GRID_PAGES_H