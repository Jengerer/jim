#include "notification_queue.hpp"

#define NOTIFICATION_ALPHA_SPEED	20
#define NOTIFICATION_QUEUE_DELAY	1500

/*
 * Notification queue generic constructor.
 */
NotificationQueue::NotificationQueue( void )
{
	// Notification created.
	next_time_ = GetTickCount();
	current_ = nullptr;
	set_size( 0, 0 );
}

/*
 * Notification queue destructor.
 */
NotificationQueue::~NotificationQueue( void )
{
	// Notification destroyed.
}

/*
 * Draw top-most notification if any.
 */
void NotificationQueue::draw( JUI::Graphics2D* graphics )
{
	// Draw top notification.
	if (has_notification()) {
		Notification *top = get_current_notification();
		top->draw( graphics );
	}
}

/*
 * Update notification state.
 */
void NotificationQueue::update_notifications( void )
{
	// Update alpha of notification.
	if (has_notification()) {
		Notification *top = get_current_notification();

        // Check if notification has expired.
		if (next_time_ <= GetTickCount()) {
			top->set_alpha( top->get_alpha() - NOTIFICATION_ALPHA_SPEED );

            // Set next after fading out.
			if (top->get_alpha() == 0) {
				set_next_notification();
			}
		}
		else {
			top->set_alpha( top->get_alpha() + NOTIFICATION_ALPHA_SPEED );
		}
	}
}

/*
 * Handle mouse movement events.
 */
JUI::IOResult NotificationQueue::on_mouse_moved( JUI::Mouse* mouse )
{
	// No mouse movement for notifications yet.
    // TODO: If mouse hovering over notification, extend.
    return JUI::IO_RESULT_UNHANDLED;
}

/*
 * Handle mouse click events.
 */
JUI::IOResult NotificationQueue::on_mouse_clicked( JUI::Mouse* mouse )
{
	// Handle touching notifications; skip to next.
	if (has_notification()) {
		Notification *current = get_current_notification();
		if (mouse->is_touching( current )) {
			set_next_notification();
            return JUI::IO_RESULT_HANDLED;
		}
	}
	
    return JUI::IO_RESULT_UNHANDLED;
}

/*
 * Handle mouse release events.
 */
JUI::IOResult NotificationQueue::on_mouse_released( JUI::Mouse* mouse )
{
	// No release handling.
    return JUI::IO_RESULT_UNHANDLED;
}

/*
 * Add notification to queue.
 */
bool NotificationQueue::add_notification( const JUTIL::String* message, const JUI::Texture *texture )
{
    Notification* notification;
    if (!JUTIL::BaseAllocator::allocate( &notification )) {
        return false;
    }
	notification = new (notification) Notification();
    if (!notification->initialize( texture )) {
        JUTIL::BaseAllocator::destroy( notification );
        return false;
    }
	notification->set_alpha( 0 );
	if (!add( notification )) {
        JUTIL::BaseAllocator::destroy( notification );
        return false;
    }

	// Add and set to next, if empty.
	notifications_.push( notification );
	if (!has_notification()) {
		set_next_notification();
	}
    return true;
}

/*
 * Move to next notification.
 */
void NotificationQueue::set_next_notification( void )
{
	// Remove previous.
	if (has_notification()) {
		remove( current_ );
        JUTIL::BaseAllocator::destroy( current_ );
	}

	// Get and set new.
	if (has_more_notifications()) {
		current_ = notifications_.front();

        // Position to align bottom-right to this position.
        int current_x = -current_->get_width();
        int current_y = -current_->get_height();
		set_constraint( current_, current_x, current_y );
		notifications_.pop();
		next_time_ = GetTickCount() + NOTIFICATION_QUEUE_DELAY;
	}
}

/*
 * Get notification currently being displayed, if any.
 */
Notification* NotificationQueue::get_current_notification( void )
{
	return current_;
}

/*
 * Check whether a notification is present and active.
 */
bool NotificationQueue::has_notification( void ) const
{
	return current_ != nullptr;
}

/*
 * Check whether any more notifications exist.
 */
bool NotificationQueue::has_more_notifications( void ) const
{
	return !notifications_.empty();
}