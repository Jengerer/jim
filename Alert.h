#pragma once

#include "Container.h"
#include "Notice.h"
#include "Button.h"
#include "Mouse.h"

class Alert: public Notice
{

public:

	Alert( const string& message );
	virtual ~Alert( void );

	void			OnDraw( DirectX* directX );
	virtual void	UpdatePosition( void );
	virtual void	Pack( void );
	virtual void	SetMessage( const string& message );
	const Button*	GetButton( void ) const;

	// Mouse handling.
	virtual bool	OnLeftClicked( Mouse *mouse );
	virtual bool	OnLeftReleased( Mouse *mouse );
	virtual bool	OnRightClicked( Mouse *mouse );
	virtual bool	OnRightReleased( Mouse *mouse );
	virtual bool	OnMouseMoved( Mouse *mouse );


private:
	Button* okButton;
};