#pragma once

#include "container.h"
#include "button.h"
#include "notice.h"
#include "mouse.h"

class Alert: public Notice
{

public:

	Alert( const std::string& message );
	virtual ~Alert( void );

	virtual void	Pack( void );
	virtual void	SetMessage( const std::string& message );
	const Button*	GetButton( void ) const;

	// Mouse handling.
	virtual bool	MouseClicked( Mouse *mouse );
	virtual bool	MouseReleased( Mouse *mouse );
	virtual bool	MouseMoved( Mouse *mouse );

private:

	Button*			okButton_;

};