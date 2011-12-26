#ifndef NOTIFICATION_QUEUE_H
#define NOTIFICATION_QUEUE_H

#include <queue>

#include <jui/constrained_container.h>
#include <jui/imouse_handler.h>
#include <jui/texture.h>

#include "notification.h"

class NotificationQueue : public ConstrainedContainer, public IMouseHandler
{

public:

	NotificationQueue( void );
	virtual ~NotificationQueue( void );

	// Drawing and container functions.
	virtual void	draw( Graphics2D* graphics );

	// Updating notification state.
	void			UpdateNotifications( void );

	// Input handling.
	virtual bool	on_mouse_moved( Mouse *mouse );
	virtual bool	on_mouse_clicked( Mouse *mouse );
	virtual bool	on_mouse_released( Mouse *mouse );

	// Notification handling.
	void			add_notification( const std::string& message, const Texture *texture );
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