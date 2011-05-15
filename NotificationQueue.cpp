#include "NotificationQueue.h"

#define NOTIFICATION_ALPHA_SPEED	20
#define NOTIFICATION_QUEUE_DELAY	1500

NotificationQueue::NotificationQueue( void )
{
	// Notification created.
	nextTime_ = GetTickCount();
	current_ = nullptr;
	SetSize( 0, 0 );
}

NotificationQueue::~NotificationQueue( void )
{
	// Notification destroyed.
}

void NotificationQueue::OnDraw( DirectX *directX )
{
	// Draw top notification.
	if ( HasNotification() ) {
		Notification *top = GetCurrentNotification();
		top->OnDraw( directX );
	}
}

void NotificationQueue::UpdatePosition( void )
{
	// Justify notification to bottom-right.
	if ( HasNotification() ) {
		Notification *top = GetCurrentNotification();
		top->SetPosition( GetX() - top->GetWidth(), GetY() - top->GetHeight() );
	}
}

void NotificationQueue::UpdateNotifications( void )
{
	// Update alpha of notification.
	if ( HasNotification() ) {
		Notification *top = GetCurrentNotification();
		if ( nextTime_ <= GetTickCount() ) {
			top->SetAlpha( top->GetAlpha() - NOTIFICATION_ALPHA_SPEED );
			if (top->GetAlpha() == 0) {
				SetNextNotification();
			}
		}
		else {
			top->SetAlpha( top->GetAlpha() + NOTIFICATION_ALPHA_SPEED );
		}
	}
}

bool NotificationQueue::OnMouseMoved( Mouse *mouse )
{
	// Mouse moved.
	return false;
}

bool NotificationQueue::OnLeftClicked( Mouse *mouse )
{
	// Left clicked.
	return false;
}

bool NotificationQueue::OnLeftReleased( Mouse *mouse )
{
	// Left released.
	return false;
}

bool NotificationQueue::OnRightClicked( Mouse *mouse )
{
	// Right clicked.
	return false;
}

bool NotificationQueue::OnRightReleased( Mouse *mouse )
{
	// Right released.
	return false;
}

void NotificationQueue::AddNotification( const string& message, Texture *texture )
{
	Notification *notification = new Notification( message, texture );
	notification->SetAlpha( 0 );
	Add( notification );

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
		Remove( current_ );
		delete current_;
		current_ = nullptr;
	}

	// Get and set new.
	if ( HasMoreNotifications() ) {
		current_ = notificationQueue_.front();
		notificationQueue_.pop();
		nextTime_ = GetTickCount() + NOTIFICATION_QUEUE_DELAY;
		UpdatePosition();
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