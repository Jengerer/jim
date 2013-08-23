#include "button_notice.hpp"

ButtonNotice::ButtonNotice( void )
{
    button_manager_.set_event_listener( this );
}

ButtonNotice::~ButtonNotice( void )
{
}

/*
 * Pass mouse movement event to buttons for hover effect.
 */
JUI::IOResult ButtonNotice::on_mouse_moved( JUI::Mouse* mouse )
{
    JUI::IOResult result = button_manager_.on_mouse_moved( mouse );
    return result;
}

/*
 * Pass mouse click event to button manager to register first click.
 */
JUI::IOResult ButtonNotice::on_mouse_clicked( JUI::Mouse* mouse )
{
    JUI::IOResult result = button_manager_.on_mouse_clicked( mouse );
    return result;
}

/*
 * Pass mouse release event to button manager.
 */
JUI::IOResult ButtonNotice::on_mouse_released( JUI::Mouse* mouse )
{
    JUI::IOResult result = button_manager_.on_mouse_released( mouse );
    return result;
}

/*
 * Default button press event handler does nothing.
 */
bool ButtonNotice::on_button_pressed( Button* button )
{
    return true;
}

/*
 * Default button release event handler does nothing.
 */
bool ButtonNotice::on_button_released( Button* button )
{
    return true;
}
