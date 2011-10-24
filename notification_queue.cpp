#include "notification_queue.h"

#define NOTIFICATION_ALPHA_SPEED	20
#define NOTIFICATION_QUEUE_DELAY	1500

NotificationQueue::NotificationQueue( void )
{
	// Notification created.
	nextTime_ = GetTickCount();
	current_ = nullptr;
	set_size( 0, 0 );
}

NotificationQueue::~NotificationQueue( void )
{
	// Notification destroyed.
}

void NotificationQueue::draw( Graphics2D* graphics )
{
	// Draw top notification.
	if ( HasNotification() ) {
		Notification *top = GetCurrentNotification();
		top->draw( graphics );
	}
}

void NotificationQueue::UpdateNotifications( void )
{
	// Update alpha of notification.
	if ( HasNotification() ) {
		Notification *top = GetCurrentNotification();
		if ( nextTime_ <= GetTickCount() ) {
			top->set_alpha( top->GetAlpha() - NOTIFICATION_ALPHA_SPEED );
			if (top->GetAlpha() == 0) {
				SetNextNotification();
			}
		}
		else {
			top->set_alpha( top->GetAlpha() + NOTIFICATION_ALPHA_SPEED );
		}
	}
}

bool NotificationQueue::on_mouse_moved( Mouse *mouse )
{
	// Mouse moved.
	return false;
}

bool NotificationQueue::on_mouse_clicked( Mouse *mouse )
{
	// Left clicked.
	if (HasNotification()) {
		Notification *current = GetCurrentNotification();
		if (mouse->is_touching( current )) {
			SetNextNotification();
			return true;
		}
	}
	
	return false;
}

bool NotificationQueue::on_mouse_released( Mouse *mouse )
{
	// Left released.
	return false;
}

void NotificationQueue::add_notification( const std::string& message, const Texture *texture )
{
	Notification *notification = new Notification( message, texture );
	notification->set_alpha( 0 );
	add( notification );

	// Add and set to next, if empty.
	notificationQueue_.push( notification );
	if (!HasNotification()) {
		SetNextNotification();
	}
}

void NotificationQueue::SetNextNotification( void )
{
	// Remove previous.
	if ( HasNotification() ) {
		remove( current_ );
		delete current_;
		current_ = nullptr;
	}

	// Get and set new.
	if ( HasMoreNotifications() ) {
		current_ = notificationQueue_.front();
		set_constraint( current_,
			static_cast<float>(-current_->get_width()), 
			static_cast<float>(-current_->get_height()) );
		notificationQueue_.pop();
		nextTime_ = GetTickCount() + NOTIFICATION_QUEUE_DELAY;
	}
}

Notification* NotificationQueue::GetCurrentNotification( void )
{
	return current_;
}

bool NotificationQueue::HasNotification( void ) const
{
	return current_ != nullptr;
}

bool NotificationQueue::HasMoreNotifications( void ) const
{
	return !notificationQueue_.empty();
}