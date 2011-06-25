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

void Popup::CenterTo( const Container* parent )
{
	SetLocalPosition( 
		floor((parent->GetWidth() - GetWidth()) / 2.0f),
		floor((parent->GetHeight() - GetHeight()) / 2.0f) );
	parent->UpdateChild( this );
}