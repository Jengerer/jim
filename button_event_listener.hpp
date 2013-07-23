#ifndef BUTTON_EVENT_LISTENER_HPP
#define BUTTON_EVENT_LISTENER_HPP

#include "button.hpp"

/*
 * Interface for handling button events.
 */
class ButtonEventListener
{

public:

    virtual bool on_button_pressed( Button* button ) = 0;
    virtual bool on_button_released( Button* button ) = 0;

};

#endif // BUTTON_EVENT_LISTENER_HPP