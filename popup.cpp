#include "popup.hpp"

/*
 * Default popup constructor.
 */
Popup::Popup( void )
{
}

/*
 * Popup constructor with default position.
 */
Popup::Popup( int x, int y )
    : JUI::ConstrainedContainer( x, y )
{
}

/*
 * Popup destructor.
 */
Popup::~Popup( void )
{
	// Popup is removed.
}

/*
 * Set popup state.
 */
void Popup::set_state( EPopupState state )
{
	state_ = state;
}

/*
 * Get popup state.
 */
EPopupState Popup::get_state( void ) const
{
	return state_;
}

/*
 * Handle mouse clicks on popup.
 * Popups always take precedence, so if we get event, return handled.
 */
JUI::IOResult Popup::on_mouse_clicked( JUI::Mouse* mouse )
{
    // Return handled if mouse is touching.
    return JUI::IO_RESULT_HANDLED;
}

/*
 * Handle mouse releases on popup.
 * Popups always take precedence, so if we get event, return handled.
 */
JUI::IOResult Popup::on_mouse_released( JUI::Mouse* mouse )
{
    return JUI::IO_RESULT_HANDLED;
}

/*
 * Handle mouse movement.
 * Popups always take precedence, so if we get event, return handled.
 */
JUI::IOResult Popup::on_mouse_moved( JUI::Mouse* mouse )
{
	// Return handled if mouse is touching.
    return JUI::IO_RESULT_HANDLED;
}

/*
 * Handle key press events.
 * Popups always take precedence, so if we get event, return handled.
 */
JUI::IOResult Popup::on_key_pressed( int key )
{
    return JUI::IO_RESULT_HANDLED;
}

/*
 * Handle key release events.
 * Popups always take precedence, so if we get event, return handled.
 */
JUI::IOResult Popup::on_key_released( int key )
{
	// Generic popup doesn't handle keys.
    return JUI::IO_RESULT_UNHANDLED;
}

/*
 * Position popup to center of container.
 */
void Popup::center_to( const Container* parent )
{
    int mid_x = parent->get_x() + (parent->get_width() - get_width()) / 2;
    int mid_y = parent->get_y() + (parent->get_height() - get_height()) / 2;
    set_position( mid_x, mid_y );
}