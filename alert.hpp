#ifndef ALERT_H
#define ALERT_H

#include <jui/layout/container.hpp>
#include <jui/io/mouse.hpp>

#include "button.hpp"
#include "notice.hpp"

class Alert: public Notice
{

public:

	Alert( const std::string& message );
	virtual ~Alert( void );

	const Button*	GetButton( void ) const;

	// Keyboard handling.
	virtual bool	on_key_released( int key );

	// JUI::Mouse* handling.
	virtual bool	on_mouse_clicked( JUI::Mouse* mouse );
	virtual bool	on_mouse_released( JUI::Mouse* mouse );
	virtual bool	on_mouse_moved( JUI::Mouse* mouse );

private:

	Button*			ok_;

};

#endif // ALERT_H