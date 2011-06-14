#include "popup.h"

Popup::Popup( float x, float y ) : Draggable( x, y )
{
}

Popup::~Popup( void )
{
	// Popup is removed.
}

void Popup::SetState( EPopupState state )
{
	state_ = state;
}

EPopupState Popup::GetState( void ) const
{
	return state_;
}