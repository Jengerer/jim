#include "rounded_rectangle_container.hpp"

const JUI::Colour& ROUNDED_RECTANGLE_DEFAULT_COLOUR = JUI::COLOUR_WHITE;

/*
 * Rounded rectangle container constructor.
 */
RoundedRectangleContainer::RoundedRectangleContainer( unsigned int radius, unsigned int padding, float x, float y ) : JUI::ConstrainedContainer( x, y )
{
	set_padding( padding );

	// Create rounded rectangle.
	rounded_rect_ = new RoundedRectangle( 
		padding_ * 2, padding_ * 2, 
		radius, ROUNDED_RECTANGLE_DEFAULT_COLOUR );
	add( rounded_rect_ );
	set_constraint( rounded_rect_, 0.0f, 0.0f );
}

/*
 * Pack contents to container.
 */
void RoundedRectangleContainer::pack( void )
{
	float padding_value = static_cast<float>(padding_);
	set_constraint( content_, padding_value, padding_value );

	// Set rectangle size.
	int rect_width = content_->get_width() + (padding_ * 2);
	int rect_height = content_->get_height() + (padding_ * 2);
	rounded_rect_->set_size( rect_width, rect_height );
	set_size( rect_width, rect_height );
}

void RoundedRectangleContainer::set_content( Component* content )
{
	content_ = content;
}

RoundedRectangle* RoundedRectangleContainer::get_rounded_rectangle() const
{
	return rounded_rect_;
}

void RoundedRectangleContainer::set_padding( unsigned int padding )
{
	padding_ = padding;
}