#ifndef NOTIFICATION_QUEUE_H
#define NOTIFICATION_QUEUE_H

#include <queue>

#include <jui/layout/constrained_container.hpp>
#include <jui/io/mouse_handler_interface.hpp>
#include <jui/gfx/texture.hpp>

#include "notification.hpp"

class NotificationQueue : public JUI::ConstrainedContainer, public JUI::MouseHandlerInterface
{

public:

	NotificationQueue( void );
	virtual ~NotificationQueue( void );

	// Drawing and container functions.
	virtual void	draw( JUI::Graphics2D* graphics );

	// Updating notification state.
	void			update_notifications( void );

	// Input handling.
	virtual bool	on_mouse_moved( JUI::Mouse* mouse );
	virtual bool	on_mouse_clicked( JUI::Mouse* mouse );
	virtual bool	on_mouse_released( JUI::Mouse* mouse );

	// Notification handling.
	void			add_notification( const JUTIL::String* message, const JUI::Texture *texture );
	void			set_next_notification( void );

private:

	Notification*	get_current_notification( void );
	bool			has_notification( void ) const;
	bool			has_more_notifications( void ) const;

private:

	DWORD next_time_;
	Notification* current_;
	std::queue<Notification*> notifications_;

};

#endif // NOTIFICATION_QUEUE_H