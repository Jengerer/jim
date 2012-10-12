#include "notification_queue.hpp"

#define NOTIFICATION_ALPHA_SPEED	20
#define NOTIFICATION_QUEUE_DELAY	1500

NotificationQueue::NotificationQueue( void )
{
	// Notification created.
	next_time_ = GetTickCount();
	current_ = nullptr;
	set_size( 0, 0 );
}

NotificationQueue::~NotificationQueue( void )
{
	// Notification destroyed.
}

void NotificationQueue::draw( JUI::Graphics2D* graphics )
{
	// Draw top notification.
	if ( has_notification() ) {
		Notification *top = get_current_notification();
		top->draw( graphics );
	}
}

void NotificationQueue::update_notifications( void )
{
	// Update alpha of notification.
	if ( has_notification() ) {
		Notification *top = get_current_notification();
		if ( next_time_ <= GetTickCount() ) {
			top->set_alpha( top->get_alpha() - NOTIFICATION_ALPHA_SPEED );
			if (top->get_alpha() == 0) {
				set_next_notification();
			}
		}
		else {
			top->set_alpha( top->get_alpha() + NOTIFICATION_ALPHA_SPEED );
		}
	}
}

JUI::IOResult NotificationQueue::on_mouse_moved( JUI::Mouse* mouse )
{
	// No mouse movement for notifications yet.
    // TODO: If mouse hovering over notification, extend.
    return JUI::IO_RESULT_UNHANDLED;
}

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

JUI::IOResult NotificationQueue::on_mouse_released( JUI::Mouse* mouse )
{
	// No release handling.
    return JUI::IO_RESULT_UNHANDLED;
}

bool NotificationQueue::add_notification( const JUTIL::String* message, const JUI::Texture *texture )
{
    Notification* notification;
    if (!JUTIL::BaseAllocator::allocate( &notification )) {
        return false;
    }
	notification = new (notification) Notification( message, texture );
    if (!notification->initialize()) {
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
}

void NotificationQueue::set_next_notification( void )
{
	// Remove previous.
	if ( has_notification() ) {
		remove( current_ );
		delete current_;
		current_ = nullptr;
	}

	// Get and set new.
	if ( has_more_notifications() ) {
		current_ = notifications_.front();
		set_constraint( current_,
			static_cast<float>(-current_->get_width()), 
			static_cast<float>(-current_->get_height()) );
		notifications_.pop();
		next_time_ = GetTickCount() + NOTIFICATION_QUEUE_DELAY;
	}
}

Notification* NotificationQueue::get_current_notification( void )
{
	return current_;
}

bool NotificationQueue::has_notification( void ) const
{
	return current_ != nullptr;
}

bool NotificationQueue::has_more_notifications( void ) const
{
	return !notifications_.empty();
}