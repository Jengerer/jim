#include "Popup.h"

Popup::Popup( float x, float y ) : Draggable( x, y )
{
	state = POPUP_STATE_ACTIVE;
}

Popup::~Popup()
{
	// Popup is removed.
}

bool Popup::mouseEvent( Mouse* mouse, EMouseEvent eventType )
{
	// Call parent mouse event.
	Container::mouseEvent( mouse, eventType );

	// Always return true.
	return true;
}

EPopupState Popup::getState() const
{
	return state;
}