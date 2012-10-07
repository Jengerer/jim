#include "rounded_rectangle_container.hpp"

const JUI::Colour& ROUNDED_RECTANGLE_DEFAULT_COLOUR = JUI::COLOUR_WHITE;
const int DEFAULT_CORNER_RADIUS = 5;

/*
 * Rounded rectangle container constructor.
 */
RoundedRectangleContainer::RoundedRectangleContainer( unsigned int padding, float x, float y ) : JUI::ConstrainedContainer( x, y )
{
	set_padding( padding );
	rounded_rect_ = nullptr;
}

/*
 * Initialize sub-elements.
 */
bool RoundedRectangleContainer::initialize( void )
{
	// Create rounded rectangle.
	if (!JUTIL::BaseAllocator::allocate( &rounded_rect_ )) {
		return false;
	}
	int default_size = padding_ * 2;
	rounded_rect_ = new (rounded_rect_) RoundedRectangle( default_size, default_size, DEFAULT_CORNER_RADIUS, &ROUNDED_RECTANGLE_DEFAULT_COLOUR );

	// Add to container.
	if (!add( rounded_rect_ )) {
		JUTIL::BaseAllocator::destroy( rounded_rect_ );
		return false;
	}
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