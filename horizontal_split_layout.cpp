#include "horizontal_split_layout.h"

HorizontalSplitLayout::HorizontalSplitLayout( int width, unsigned int spacing )
{
	set_size( width, 0 );

	// Create layouts.
	left_ = new HorizontalLayout( spacing, ALIGN_TOP );
	right_ = new HorizontalLayout( spacing, ALIGN_TOP );
	set_constraint( left_, 0.0f, 0.0f );
	set_constraint( right_, static_cast<float>(width), 0.0f );

	add( left_ );
	add( right_ );
}

void HorizontalSplitLayout::pack()
{
	left_->pack();
	right_->pack();
	set_constraint( right_,
		static_cast<float>(get_width() - right_->get_width()), 0.0f );

	// Set height as maximum.
	int leftHeight = left_->get_height();
	int rightHeight = right_->get_height();
	int maxHeight = leftHeight > rightHeight ? leftHeight : rightHeight;
	set_size( get_width(), maxHeight );
}

void HorizontalSplitLayout::add_left( Component* component )
{
	left_->add( component );
}

void HorizontalSplitLayout::add_right( Component* component )
{
	right_->add( component );
}