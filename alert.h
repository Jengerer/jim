#pragma once

#include "container.h"
#include "label_button.h"
#include "notice.h"
#include "mouse.h"

class Alert: public Notice
{

public:

	Alert( const string& message );
	virtual ~Alert( void );

	void			OnDraw( DirectX* directX );
	virtual void	Pack( void );
	virtual void	SetMessage( const string& message );
	const Button*	GetButton( void ) const;

	// Mouse handling.
	virtual bool	OnLeftClicked( Mouse *mouse );
	virtual bool	OnLeftReleased( Mouse *mouse );
	virtual bool	OnMouseMoved( Mouse *mouse );


private:

	LabelButton *okButton_;

};