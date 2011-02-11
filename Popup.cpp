#include "Popup.h"

Popup::Popup( float x, float y ) : Draggable( x, y )
{
	setState( POPUP_STATE_ACTIVE );
}

Popup::~Popup()
{
	// Popup is removed.
}

void Popup::setState( EPopupState state )
{
	state_ = state;
}

EPopupState Popup::getState() const
{
	return state_;
}