#ifndef CONFIRMATION_H
#define CONFIRMATION_H

#include <jui/io/mouse.hpp>
#include "button_notice.hpp"

enum ConfirmationResponse
{
	RESPONSE_NULL,
	RESPONSE_YES,
	RESPONSE_NO
};

/*
 * UI object that will ask the user a boolean question and return
 * response when polled/killed.
 */
class Confirmation: public ButtonNotice
{

public:

	Confirmation( void );
	virtual ~Confirmation( void );

    // Layout management.
    virtual bool initialize( const JUTIL::String* question );

    // Get response on button press.
	ConfirmationResponse get_response( void ) const;

    // Button release event handling.
    virtual bool on_button_released( Button* button );

private:

	ConfirmationResponse response_;
	Button* yes_button_;
	Button* no_button_;

};

#endif // CONFIRMATION_H