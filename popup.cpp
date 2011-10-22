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

bool Popup::on_mouse_clicked( Mouse* mouse )
{
	return mouse->IsTouching( this );
}

bool Popup::on_mouse_released( Mouse* mouse )
{
	return mouse->IsTouching( this );
}

bool Popup::on_mouse_moved( Mouse* mouse )
{
	return mouse->IsTouching( this );
}

bool Popup::on_key_pressed( Key key )
{
	return false;
}

bool Popup::on_key_released( Key key )
{
	return false;
}

void Popup::CenterTo( const Container* parent )
{
	SetPosition( 
		parent->GetX() + floor((parent->GetWidth() - GetWidth()) / 2.0f),
		parent->GetY() + floor((parent->GetHeight() - GetHeight()) / 2.0f) );
}