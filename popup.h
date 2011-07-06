#pragma once

#include "container.h"
#include "draggable.h"

enum EPopupState
{
	POPUP_STATE_ACTIVE,
	POPUP_STATE_HIDDEN,
	POPUP_STATE_KILLED,
};

class Popup: public ConstrainedContainer, public IMouseHandler
{
public:

	Popup( float x = 0.0f, float y = 0.0f );
	virtual ~Popup();

	// Get activity state.
	void			SetState( EPopupState state );
	EPopupState		GetState( void ) const;

	// Mouse handling.
	virtual bool	MouseClicked( Mouse* mouse );
	virtual bool	MouseReleased( Mouse* mouse );
	virtual bool	MouseMoved( Mouse* mouse );

	// Position alignment.
	void			CenterTo( const Container* parent );

private:
	EPopupState		state_;
};