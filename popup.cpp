#include "popup.h"

Popup::Popup( float x, float y ) : ConstrainedContainer( x, y )
{
}

Popup::~Popup( void )
{
	// Popup is removed.
}

void Popup::set_state( EPopupState state )
{
	state_ = state;
}

EPopupState Popup::get_state( void ) const
{
	return state_;
}

bool Popup::is_active( void ) const
{
	return state_ == POPUP_STATE_ACTIVE;
}

bool Popup::is_hidden( void ) const
{
	return state_ == POPUP_STATE_HIDDEN;
}

bool Popup::is_killed( void ) const
{
	return state_ == POPUP_STATE_KILLED;
}

bool Popup::on_mouse_clicked( Mouse* mouse )
{
	return mouse->is_touching( this );
}

bool Popup::on_mouse_released( Mouse* mouse )
{
	return mouse->is_touching( this );
}

bool Popup::on_mouse_moved( Mouse* mouse )
{
	return mouse->is_touching( this );
}

bool Popup::on_key_pressed( Key key )
{
	return false;
}

bool Popup::on_key_released( Key key )
{
	return false;
}

void Popup::center_to( const Container* parent )
{
	set_position( 
		parent->get_x() + floor((parent->get_width() - get_width()) / 2.0f),
		parent->get_y() + floor((parent->get_height() - get_height()) / 2.0f) );
}