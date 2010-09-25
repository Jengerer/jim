#include "Popup.h"

Popup::Popup()
{
	state = POPUP_STATE_ACTIVE;
}

EPopupState Popup::getState() const
{
	return state;
}