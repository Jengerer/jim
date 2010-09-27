#pragma once

#include "Drawable.h"

enum EPopupState
{
	POPUP_STATE_ACTIVE,
	POPUP_STATE_INACTIVE
};

class Popup: public Drawable
{
public:
	Popup();

	// Get activity state.
	EPopupState		getState() const;

protected:
	EPopupState		state;
};