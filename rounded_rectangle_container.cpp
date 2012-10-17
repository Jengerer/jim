#include "rounded_rectangle_container.hpp"

const JUI::Colour& ROUNDED_RECTANGLE_DEFAULT_COLOUR = JUI::COLOUR_WHITE;
const int DEFAULT_CORNER_RADIUS = 5;

/*
 * Rounded rectangle container constructor.
 */
RoundedRectangleContainer::RoundedRectangleContainer( unsigned int padding )
{
	set_padding( padding );
	rounded_rect_ = nullptr;
}

/*
 * Rounded rectangle container constructor with default position.
 */
RoundedRectangleContainer::RoundedRectangleContainer( unsigned int padding, int x, int y ) 
    : JUI::ConstrainedContainer( x, y )
{
	set_padding( padding );
	rounded_rect_ = nullptr;
}

/*
 * Rounded rectangle container destructor.
 */
RoundedRectangleContainer::~RoundedRectangleContainer( void )
{
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
	set_constraint( rounded_rect_, 0, 0 );
    return true;
}

/*
 * Pack contents to container.
 */
void RoundedRectangleContainer::pack( void )
{
	set_constraint( content_, padding_, padding_ );

	// Set rectangle size.
	int rect_width = content_->get_width() + (padding_ * 2);
	int rect_height = content_->get_height() + (padding_ * 2);
	rounded_rect_->set_size( rect_width, rect_height );
	set_size( rect_width, rect_height );
}

/*
 * Set content shown in the rounded container.
 */
void RoundedRectangleContainer::set_content( Component* content )
{
	content_ = content;
}

/*
 * Get the rounded rectangle object used by the container.
 */
RoundedRectangle* RoundedRectangleContainer::get_rounded_rectangle() const
{
	return rounded_rect_;
}

/*
 * Set rounded rectangle container padding.
 */
void RoundedRectangleContainer::set_padding( unsigned int padding )
{
	padding_ = padding;
}