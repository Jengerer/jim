#ifndef CONFIRMATION_H
#define CONFIRMATION_H

#include <jui/io/mouse.hpp>

#include "notice.hpp"
#include "button.hpp"

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
class Confirmation: public Notice
{

public:

	Confirmation( void );
	virtual ~Confirmation( void );

    // Layout management.
    virtual bool initialize( const JUTIL::String* question );

    // Get response on button press.
	ConfirmationResponse	get_response( void ) const;

    // Mouse handling functions.
	virtual bool			on_mouse_released( JUI::Mouse* mouse );
	virtual bool			on_mouse_moved( JUI::Mouse* mouse );

private:

	ConfirmationResponse response_;
	Button* yes_;
	Button* no_;

};

#endif // CONFIRMATION_H