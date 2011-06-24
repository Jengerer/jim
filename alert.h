#pragma once

#include "container.h"
#include "button.h"
#include "notice.h"
#include "mouse.h"

class Alert: public Notice
{

public:

	Alert( const string& message );
	virtual ~Alert( void );

	virtual void	Pack( void );
	virtual void	SetMessage( const string& message );
	const Button*	GetButton( void ) const;

	// Mouse handling.
	virtual bool	OnLeftClicked( Mouse *mouse );
	virtual bool	OnLeftReleased( Mouse *mouse );
	virtual bool	OnMouseMoved( Mouse *mouse );


private:

	Button*			okButton_;

};