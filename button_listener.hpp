#ifndef BUTTON_LISTENER_HPP
#define BUTTON_LISTENER_HPP

#include "button.hpp"

/*
 * Interface for handling button events.
 */
class ButtonListener
{

public:

    virtual bool on_button_pressed( Button* button ) = 0;
    virtual bool on_button_released( Button* button ) = 0;

};

#endif // BUTTON_LISTENER_HPP