#include "Popup.h"

Popup::Popup( float x, float y ) : Draggable( x, y )
{
	state = POPUP_STATE_ACTIVE;
}

Popup::~Popup()
{
	// Popup is removed.
}

EPopupState Popup::getState() const
{
	return state;
}

bool Popup::mouseEvent( Mouse *mouse, EMouseEvent mouseEvent )
{
	// Popup always take precedence.
	Component::mouseEvent( mouse, mouseEvent );
	return true;
}