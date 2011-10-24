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

	void add_pages( const SlotVector* slots );

	void pack();

	SlotView* get_touching_slot( Mouse* mouse ) const;
	SlotGridView* get_current_page() const;

	unsigned int get_page_index() const;
	unsigned int get_page_count() const;

	bool next_page();
	bool previous_page();
	void update_offset();
	virtual void update_view();

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