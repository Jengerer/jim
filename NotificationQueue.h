#ifndef NOTIFICATION_QUEUE_H
#define NOTIFICATION_QUEUE_H

#include <queue>
#include <windows.h>

#include "Container.h"
#include "DirectX.h"
#include "IMouseHandler.h"
#include "Notification.h"
#include "Texture.h"

class NotificationQueue : public Container, public IMouseHandler
{

public:

	NotificationQueue( void );
	virtual ~NotificationQueue( void );

	// Drawing and container functions.
	virtual void	OnDraw( DirectX* directX );
	virtual void	UpdatePosition( void );

	// Updating notification state.
	void			UpdateNotifications( void );

	// Input handling.
	virtual bool	OnMouseMoved( Mouse *mouse );
	virtual bool	OnLeftClicked( Mouse *mouse );
	virtual bool	OnLeftReleased( Mouse *mouse );
	virtual bool	OnRightClicked( Mouse *mouse );
	virtual bool	OnRightReleased( Mouse *mouse );

	// Notification handling.
	void			AddNotification( const string& message, Texture *texture );
	void			SetNextNotification( void );

private:

	Notification*	GetCurrentNotification( void );
	bool			HasNotification( void ) const;
	bool			HasMoreNotifications( void ) const;

private:

	DWORD						nextTime_;
	Notification*				current_;
	std::queue<Notification*>	notificationQueue_;

};

#endif // NOTIFICATION_QUEUE_H