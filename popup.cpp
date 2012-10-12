#include "popup.hpp"

Popup::Popup( float x, float y ) : JUI::ConstrainedContainer( x, y )
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

JUI::IOResult Popup::on_mouse_clicked( JUI::Mouse* mouse )
{
    // Return handled if mouse is touching.
    if (mouse->is_touching( this )) {
        return JUI::IO_RESULT_HANDLED;
    }

    return JUI::IO_RESULT_UNHANDLED;
}

JUI::IOResult Popup::on_mouse_released( JUI::Mouse* mouse )
{
	// Return handled if mouse is touching.
    if (mouse->is_touching( this )) {
        return JUI::IO_RESULT_HANDLED;
    }

    return JUI::IO_RESULT_UNHANDLED;
}

JUI::IOResult Popup::on_mouse_moved( JUI::Mouse* mouse )
{
	// Return handled if mouse is touching.
    if (mouse->is_touching( this )) {
        return JUI::IO_RESULT_HANDLED;
    }

    return JUI::IO_RESULT_UNHANDLED;
}

JUI::IOResult Popup::on_key_pressed( int key )
{
    // Generic popup doesn't handle keys.
    return JUI::IO_RESULT_UNHANDLED;
}

JUI::IOResult Popup::on_key_released( int key )
{
	// Generic popup doesn't handle keys.
    return JUI::IO_RESULT_UNHANDLED;
}

void Popup::center_to( const Container* parent )
{
	set_position( 
		parent->get_x() + floor((parent->get_width() - get_width()) / 2.0f),
		parent->get_y() + floor((parent->get_height() - get_height()) / 2.0f) );
}