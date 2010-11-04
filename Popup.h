#pragma once

#include "Container.h"
#include "Draggable.h"

enum EPopupState
{
	POPUP_STATE_ACTIVE,
	POPUP_STATE_INACTIVE
};

class Popup: public Draggable
{
public:
	Popup( float x = 0.0f, float y = 0.0f );
	virtual ~Popup();

	// Get activity state.
	EPopupState		getState() const;

protected:
	EPopupState		state;
};