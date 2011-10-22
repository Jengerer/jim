#ifndef CONFIRMATION_H
#define CONFIRMATION_H

#include "notice.h"
#include "mouse.h"
#include "button.h"

enum ConfirmationResponse
{
	RESPONSE_NULL,
	RESPONSE_YES,
	RESPONSE_NO
};

class Confirmation: public Notice
{

public:

	Confirmation( const std::string& question );
	virtual ~Confirmation( void );

	ConfirmationResponse	get_response( void ) const;

	virtual bool			on_mouse_released( Mouse* mouse );
	virtual bool			on_mouse_moved( Mouse* mouse );

private:

	ConfirmationResponse response_;
	Button* yes_;
	Button* no_;

};

#endif // CONFIRMATION_H