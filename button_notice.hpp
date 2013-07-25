#ifndef BUTTON_NOTICE_HPP
#define BUTTON_NOTICE_HPP

#include "button_manager.hpp"
#include "notice.hpp"

/*
 * Generic class for a notice that contains at least one button.
 */
class ButtonNotice : public Notice
{

public:

    ButtonNotice( void );
    virtual ~ButtonNotice( void );

    // Mouse event handling to delegate to button manager.
    virtual JUI::IOResult on_mouse_moved( JUI::Mouse* mouse );
    virtual JUI::IOResult on_mouse_clicked( JUI::Mouse* mouse );
    virtual JUI::IOResult on_mouse_released( JUI::Mouse* mouse );

    // Button manager event handling.
    virtual bool on_button_pressed( Button* button );
    virtual bool on_button_released( Button* button );

protected:

    ButtonManager button_manager_;

};

#endif // BUTTON_NOTICE_HPP