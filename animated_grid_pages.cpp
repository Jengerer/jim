#include "animated_grid_pages.h"

const float SPRING_DAMPING			= 0.15f;
const float SPRING_DECAY			= 0.65f;
const float STOP_SPEED_THRESHOLD	= 1.0f;
const float STOP_DISTANCE_THRESHOLD	= 1.0f;

AnimatedGridPages::AnimatedGridPages( unsigned int pageWidth, unsigned int pageHeight,
	unsigned int pageSpacing, unsigned int slotSpacing ) : SlotGridPages( pageWidth, pageHeight, pageSpacing, slotSpacing )
{
	viewSpeed_ = 0.0f;
	viewPosition_ = viewOffset_;
}

void AnimatedGridPages::update_view()
{
	// Move view to end.
	float viewDistance = viewOffset_ - viewPosition_;
	viewSpeed_ += viewDistance * SPRING_DAMPING;
	viewSpeed_ *= SPRING_DECAY;

	// Check if we're slowing.
	if (abs( viewDistance ) < STOP_DISTANCE_THRESHOLD && abs( viewSpeed_ ) < STOP_SPEED_THRESHOLD) {
		viewPosition_ = viewOffset_;
	}
	else {
		viewPosition_ += viewSpeed_;
	}

	pagesConstraint_->set_constraint( viewPosition_, pagesConstraint_->get_constraint_y() );
	apply_constraint( pagesConstraint_ );
}