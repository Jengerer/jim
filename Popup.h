#pragma once

#include "Container.h"
#include "Draggable.h"

enum EPopupState
{
	POPUP_STATE_ACTIVE,
	POPUP_STATE_INACTIVE,
	POPUP_STATE_KILLED,
};

class Popup: public Draggable
{
public:
	Popup( float x = 0.0f, float y = 0.0f );
	virtual ~Popup();

	// Get activity state.
	void			setState( EPopupState state );
	EPopupState		getState() const;

private:
	EPopupState		state_;
};