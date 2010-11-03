#include "Popup.h"

Popup::Popup( float x, float y ) : Draggable( x, y )
{
	state = POPUP_STATE_ACTIVE;
}

EPopupState Popup::getState() const
{
	return state;
}