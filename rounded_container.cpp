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
	set_constraint( roundedRect_, 0.0f, 0.0f );
}

void RoundedRectangleContainer::pack( void )
{
	float padding_value = static_cast<float>(padding_);
	set_constraint( content_, padding_value, padding_value );

	// Set rectangle size.
	int rectWidth = content_->get_width() + (padding_ * 2);
	int rectHeight = content_->get_height() + (padding_ * 2);
	roundedRect_->set_size( rectWidth, rectHeight );
	set_size( rectWidth, rectHeight );
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