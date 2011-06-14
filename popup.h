#pragma once

#include "container.h"
#include "draggable.h"

enum EPopupState
{
	POPUP_STATE_ACTIVE,
	POPUP_STATE_HIDDEN,
	POPUP_STATE_KILLED,
};

class Popup: public Draggable
{
public:

	Popup( float x = 0.0f, float y = 0.0f );
	virtual ~Popup();

	// Get activity state.
	void			SetState( EPopupState state );
	EPopupState		GetState( void ) const;

private:
	EPopupState		state_;
};