#include "popup.h"

Popup::Popup( float x, float y ) : ConstrainedContainer( x, y )
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

bool Popup::MouseClicked( Mouse* mouse )
{
	return mouse->IsTouching( this );
}

bool Popup::MouseReleased( Mouse* mouse )
{
	return mouse->IsTouching( this );
}

bool Popup::MouseMoved( Mouse* mouse )
{
	return mouse->IsTouching( this );
}

void Popup::CenterTo( const Container* parent )
{
	SetPosition( 
		parent->GetX() + floor((parent->GetWidth() - GetWidth()) / 2.0f),
		parent->GetY() + floor((parent->GetHeight() - GetHeight()) / 2.0f) );
}