#ifndef BUTTON_MANAGER_HPP
#define BUTTON_MANAGER_HPP

#include <containers/vector.hpp>
#include "button.hpp"
#include "button_event_listener.hpp"

/*
 * Button event management class.
 */
class ButtonManager : public JUI::MouseList
{

public:

    ButtonManager( ButtonEventListener* listener );
    ~ButtonManager( void );

    // Button creation.
    Button* create_button( const JUTIL::String* label, const JUI::Texture* icon );

    // Button management starting/ending.
    bool add_button( Button* button );
    void remove_button( Button* button );

private:

    JUTIL::Vector<Button*> buttons_;
    ButtonEventListener* listener_;

};

#endif // BUTTON_MANAGER_HPP