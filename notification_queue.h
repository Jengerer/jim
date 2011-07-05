#ifndef NOTIFICATION_QUEUE_H
#define NOTIFICATION_QUEUE_H

#include <queue>

#include "constrained_container.h"
#include "directx.h"
#include "imouse_handler.h"
#include "notification.h"
#include "texture.h"

class NotificationQueue : public ConstrainedContainer, public IMouseHandler
{

public:

	NotificationQueue( void );
	virtual ~NotificationQueue( void );

	// Drawing and container functions.
	virtual void	Draw( DirectX* directX );

	// Updating notification state.
	void			UpdateNotifications( void );

	// Input handling.
	virtual bool	OnMouseMoved( Mouse *mouse );
	virtual bool	OnLeftClicked( Mouse *mouse );
	virtual bool	OnLeftReleased( Mouse *mouse );

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