#include "animated_book_view.hpp"

const float DEFAULT_VIEW_SPEED		= 0.0f;
const float SPRING_DAMPING			= 0.15f;
const float SPRING_DECAY			= 0.65f;
const float STOP_SPEED_THRESHOLD	= 1.0f;
const float STOP_DISTANCE_THRESHOLD	= 1.0f;

/*
 * Animated book view constructor.
 */
AnimatedBookView::AnimatedBookView( const SlotBook* slot_book,
	unsigned int page_spacing,
	unsigned int slot_spacing ) : SlotBookView( slot_book, page_spacing, slot_spacing )
{
	set_view_speed( DEFAULT_VIEW_SPEED );
	set_view_position( get_view_offset() );
}

/*
 * Update animated book view.
 */
void AnimatedBookView::update_view()
{
	// Move view to end.
	float view_distance = get_view_offset() - get_view_position();
	
	// Add springed speed.
	float speed = get_view_speed();
	speed += view_distance * SPRING_DAMPING;
	speed *= SPRING_DECAY;
	set_view_speed( speed );
	
	// Check if we're slowing.
	if (abs( view_distance ) < STOP_DISTANCE_THRESHOLD && abs( speed ) < STOP_SPEED_THRESHOLD) {
		set_view_position( get_view_offset() );
		set_view_speed( 0.0f );
	}
	else {
		set_view_position( get_view_position() + speed );
	}

	// Update constraint.
	pages_constraint_->set_constraint( get_view_position(), 0.0f );
	apply_constraint( pages_constraint_ );
}

/*
 * Get view spring speed.
 */
float AnimatedBookView::get_view_speed( void ) const
{
	return view_speed_;
}

/*
 * Set view spring speed.
 */
void AnimatedBookView::set_view_speed( float view_speed )
{
	view_speed_ = view_speed;
}

/*
 * Get view spring position.
 */
float AnimatedBookView::get_view_position( void ) const
{
	return view_position_;
}

/*
 * Set view spring position.
 */
void AnimatedBookView::set_view_position( float view_position )
{
	view_position_ = view_position;
}