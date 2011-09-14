#ifndef NOTIFICATION_QUEUE_H
#define NOTIFICATION_QUEUE_H

#include <queue>

#include "constrained_container.h"
#include "imouse_handler.h"
#include "notification.h"
#include "texture.h"

class NotificationQueue : public ConstrainedContainer, public IMouseHandler
{

public:

	NotificationQueue( void );
	virtual ~NotificationQueue( void );

	// Drawing and container functions.
	virtual void	Draw( Graphics2D* graphics );

	// Updating notification state.
	void			UpdateNotifications( void );

	// Input handling.
	virtual bool	MouseMoved( Mouse *mouse );
	virtual bool	MouseClicked( Mouse *mouse );
	virtual bool	MouseReleased( Mouse *mouse );

	// Notification handling.
	void			AddNotification( const string& message, const Texture *texture );
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