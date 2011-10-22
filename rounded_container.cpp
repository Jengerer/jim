#include "rounded_container.h"

const Colour& ROUNDED_RECTANGLE_DEFAULT_COLOUR = COLOUR_WHITE;

RoundedRectangleContainer::RoundedRectangleContainer( unsigned int radius, unsigned int padding, float x, float y ) : ConstrainedContainer( x, y )
{
	SetPadding( padding );

	// Create rounded rectangle.
	roundedRect_ = new RoundedRectangle( 
		padding_ * 2, padding_ * 2, 
		radius, ROUNDED_RECTANGLE_DEFAULT_COLOUR );
	add( roundedRect_ );
	SetConstraint( roundedRect_, 0.0f, 0.0f );
}

void RoundedRectangleContainer::Pack( void )
{
	SetConstraint( content_, padding_, padding_ );

	// Set rectangle size.
	int rectWidth = content_->GetWidth() + (padding_ * 2);
	int rectHeight = content_->GetHeight() + (padding_ * 2);
	roundedRect_->SetSize( rectWidth, rectHeight );
	SetSize( rectWidth, rectHeight );
}

void RoundedRectangleContainer::SetContent( Component* content )
{
	content_ = content;
}

RoundedRectangle* RoundedRectangleContainer::GetRoundedRectangle() const
{
	return roundedRect_;
}

void RoundedRectangleContainer::SetPadding( unsigned int padding )
{
	padding_ = padding;
}