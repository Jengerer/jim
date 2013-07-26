#ifndef BUTTON_MANAGER_HPP
#define BUTTON_MANAGER_HPP

#include <containers/vector.hpp>
#include "button.hpp"
#include "button_listener.hpp"

/*
 * Button event management class.
 */
class ButtonManager : public JUI::MouseHandlerInterface
{

public:

    ButtonManager( void );
    ~ButtonManager( void );

    // Button event management functions.
    void set_event_listener( ButtonListener* listener );

    // Button creation.
    Button* create( const JUTIL::String* label, const JUI::Texture* icon );

    // Button management starting/ending.
    bool add( Button* button );
    bool remove( Button* button );

    // Mouse handler functions.
    virtual JUI::IOResult on_mouse_moved( JUI::Mouse* mouse );
    virtual JUI::IOResult on_mouse_clicked( JUI::Mouse* mouse );
    virtual JUI::IOResult on_mouse_released( JUI::Mouse* mouse );

private:

    Button* clicked_;
    JUTIL::Vector<Button*> buttons_;
    ButtonListener* listener_;

};

#endif // BUTTON_MANAGER_HPP