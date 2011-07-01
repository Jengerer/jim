#include "slot_grid_pages.h"

SlotGridPages::SlotGridPages( const SlotVector* slots,
	unsigned int width, unsigned int height,
	unsigned int pageSpacing, unsigned int slotSpacing )
{
	page_ = 0;
	pagesLayout_ = new HorizontalLayout( pageSpacing, ALIGN_TOP );
	pagesLayout_->SetParent( this );

	// Add grid views per page.
	unsigned int pageSize = width * height;
	unsigned int maxIndex = slots->GetSlotCount();
	for (unsigned int startIndex = 0; startIndex < maxIndex; startIndex += pageSize) {
		// Create view for this page.
 		SlotGridView* gridView = new SlotGridView( width, slotSpacing );
		gridView->AddSlots( slots, startIndex, startIndex + pageSize );
		slotGridViews_.push_back( gridView );

		pagesLayout_->Add( gridView );
	}

	Add( pagesLayout_ );
	Pack();
}

void SlotGridPages::Pack()
{
	pagesLayout_->Pack();
	
	// Set size as the first in the set.
	unsigned int width;
	unsigned int height;
	if (!slotGridViews_.empty()) {
		Component* front = slotGridViews_.front();
		width = front->GetWidth();
		height = front->GetHeight();
	}
	else {
		width = 0;
		height = 0;
	}

	SetSize( width, height );
}

Slot* SlotGridPages::GetTouchingSlot( Mouse* mouse ) const
{
	SlotGridView* currentPage = GetCurrentPage();
	return currentPage->GetTouchingSlot( mouse );
}

SlotGridView* SlotGridPages::GetCurrentPage() const
{
	if (page_ >= slotGridViews_.size()) {
		return nullptr;
	}

	return slotGridViews_[page_];
}

void SlotGridPages::NextPage()
{
	if (page_ < slotGridViews_.size() - 1) {
		++page_;
	}
}

void SlotGridPages::PreviousPage()
{
	if (page_ > 0) {
		--page_;
	}
}

void SlotGridPages::UpdateView()
{
	if (page_ < slotGridViews_.size()) {
		pagesLayout_->SetLocalPosition( -slotGridViews_[page_]->GetLocalX(), 0.0f );
		UpdateChild( pagesLayout_ );
	}
}