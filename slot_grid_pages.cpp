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
	pagesLayout_->SetParent( this );
	Add( pagesLayout_ );
	pagesConstraint_ = SetConstraint( pagesLayout_, viewOffset_, 0.0f );
	Pack();
}

void SlotGridPages::AddPages( const SlotVector* slots )
{
	// Add grid views per page.
	unsigned int pageSize = pageWidth_ * pageHeight_;
	unsigned int startIndex = slotGridViews_.size() * pageSize;
	unsigned int endIndex = slots->GetSlotCount();
	for (unsigned int startIndex = 0; startIndex < endIndex; startIndex += pageSize) {
		// Create view for this page.
 		SlotGridView* gridView = new SlotGridView( pageWidth_, slotSpacing_ );
		gridView->AddSlots( slots, startIndex, startIndex + pageSize );
		slotGridViews_.push_back( gridView );
		pagesLayout_->Add( gridView );
	}

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

SlotView* SlotGridPages::GetTouchingSlot( Mouse* mouse ) const
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

bool SlotGridPages::NextPage()
{
	if (page_ < slotGridViews_.size() - 1) {
		++page_;
		UpdateOffset();
		return true;
	}

	return false;
}

bool SlotGridPages::PreviousPage()
{
	if (page_ > 0) {
		--page_;
		UpdateOffset();
		return true;
	}

	return false;
}

void SlotGridPages::UpdateOffset()
{
	if (!slotGridViews_.empty()) {
		viewOffset_ = pagesLayout_->GetX() - GetCurrentPage()->GetX();
	}
}

void SlotGridPages::UpdateView()
{
	pagesConstraint_->SetConstraint( viewOffset_, pagesConstraint_->GetConstraintY() );
	ApplyConstraint( pagesConstraint_ );
}