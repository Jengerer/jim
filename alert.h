#ifndef ALERT_H
#define ALERT_H

#include <jui/container.h>
#include <jui/mouse.h>

#include "button.h"
#include "notice.h"

class Alert: public Notice
{

public:

	Alert( const std::string& message );
	virtual ~Alert( void );

	const Button*	GetButton( void ) const;

	// Keyboard handling.
	virtual bool	on_key_released( int key );

	// Mouse handling.
	virtual bool	on_mouse_clicked( Mouse *mouse );
	virtual bool	on_mouse_released( Mouse *mouse );
	virtual bool	on_mouse_moved( Mouse *mouse );

private:

	Button*			ok_;

};

#endif // ALERT_H