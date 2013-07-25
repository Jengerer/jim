#ifndef ALERT_HPP
#define ALERT_HPP

#include <jui/layout/container.hpp>
#include <jui/io/mouse.hpp>
#include "button_notice.hpp"

class Alert: public ButtonNotice
{

public:

	Alert( void );
	virtual ~Alert( void );

    // Layout management.
    virtual bool initialize( const JUTIL::String* message );

	// Keyboard handling.
	virtual JUI::IOResult on_key_released( int key );

    // Alert OK button event handlers.
    virtual bool on_button_released( Button* button );

private:

	Button* ok_button_;

};

#endif // ALERT_HPP