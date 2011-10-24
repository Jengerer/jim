#include "slot_grid_pages.h"

SlotGridPages::SlotGridPages( unsigned int pageWidth, unsigned int pageHeight,
	unsigned int pageSpacing, unsigned int slotSpacing )
{
	page_ = 0;
	pageWidth_ = pageWidth;
	pageHeight_ = pageHeight;
	pageSpacing_ = pageSpacing;
	slotSpacing_ = slotSpacing;
	viewOffset_ = 0.0f;

	pagesLayout_ = new HorizontalLayout( pageSpacing, ALIGN_TOP );
	pagesLayout_->set_parent( this );
	add( pagesLayout_ );
	pagesConstraint_ = set_constraint( pagesLayout_, viewOffset_, 0.0f );
	pack();
}

void SlotGridPages::add_pages( const SlotVector* slots )
{
	// Add grid views per page.
	unsigned int pageSize = pageWidth_ * pageHeight_;
	unsigned int endIndex = slots->get_slot_count();
	for (unsigned int startIndex = slotGridViews_.size() * pageSize; startIndex < endIndex; startIndex += pageSize) {
		// Create view for this page.
 		SlotGridView* gridView = new SlotGridView( pageWidth_, slotSpacing_ );
		gridView->add_slots( slots, startIndex, startIndex + pageSize );
		slotGridViews_.push_back( gridView );
		pagesLayout_->add( gridView );
	}

	pack();
}

void SlotGridPages::pack()
{
	pagesLayout_->pack();
	
	// Set size as the first in the set.
	unsigned int width;
	unsigned int height;
	if (!slotGridViews_.empty()) {
		Component* front = slotGridViews_.front();
		width = front->get_width();
		height = front->get_height();
	}
	else {
		width = 0;
		height = 0;
	}

	set_size( width, height );
}

SlotView* SlotGridPages::get_touching_slot( Mouse* mouse ) const
{
	SlotGridView* currentPage = get_current_page();
	return currentPage->get_touching_slot( mouse );
}

SlotGridView* SlotGridPages::get_current_page() const
{
	if (page_ >= slotGridViews_.size()) {
		return nullptr;
	}

	return slotGridViews_[page_];
}

unsigned int SlotGridPages::get_page_index() const
{
	return page_;
}

unsigned int SlotGridPages::get_page_count() const
{
	return slotGridViews_.size();
}

bool SlotGridPages::next_page()
{
	if (page_ < slotGridViews_.size() - 1) {
		++page_;
		update_offset();
		return true;
	}

	return false;
}

bool SlotGridPages::previous_page()
{
	if (page_ > 0) {
		--page_;
		update_offset();
		return true;
	}

	return false;
}

void SlotGridPages::update_offset()
{
	if (!slotGridViews_.empty()) {
		viewOffset_ = pagesLayout_->get_x() - get_current_page()->get_x();
	}
}

void SlotGridPages::update_view()
{
	pagesConstraint_->set_constraint( viewOffset_, pagesConstraint_->get_constraint_y() );
	apply_constraint( pagesConstraint_ );
}