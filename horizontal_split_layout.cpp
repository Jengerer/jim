#include "horizontal_split_layout.h"

HorizontalSplitLayout::HorizontalSplitLayout( int width, unsigned int spacing )
{
	SetSize( width, 0 );

	// Create layouts.
	left_ = new HorizontalLayout( spacing, ALIGN_TOP );
	right_ = new HorizontalLayout( spacing, ALIGN_TOP );
	SetConstraint( left_, 0.0f, 0.0f );
	SetConstraint( right_, width, 0.0f );

	Add( left_ );
	Add( right_ );
}

void HorizontalSplitLayout::Pack()
{
	left_->Pack();
	right_->Pack();
	SetConstraint( right_, GetWidth() - right_->GetWidth(), 0.0f );

	// Set height as maximum.
	int leftHeight = left_->GetHeight();
	int rightHeight = right_->GetHeight();
	int maxHeight = leftHeight > rightHeight ? leftHeight : rightHeight;
	SetSize( GetWidth(), maxHeight );
}

void HorizontalSplitLayout::AddLeft( Component* component )
{
	left_->Add( component );
}

void HorizontalSplitLayout::AddRight( Component* component )
{
	right_->Add( component );
}