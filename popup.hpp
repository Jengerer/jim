#ifndef POPUP_H
#define POPUP_H

#include <jui/io/mouse_handler_interface.hpp>
#include <jui/io/keyboard_handler_interface.hpp>
#include <jui/layout/vertical_layout.hpp>
#include "rounded_rectangle_container.hpp"

enum PopupState
{
	POPUP_STATE_ACTIVE,
	POPUP_STATE_HIDDEN,
	POPUP_STATE_KILLED,
};

class Popup: public RoundedRectangleContainer, public JUI::MouseHandlerInterface, public JUI::KeyboardHandlerInterface
{
public:

    Popup( void );
	Popup( int x, int y );
	virtual ~Popup( void );

    // UI initialization.
    bool initialize( void );
    virtual void pack( void );

	// Get activity state.
	void set_state( PopupState state );
	PopupState get_state( void ) const;

	// Mouse event handling.
    virtual JUI::IOResult on_mouse_moved( JUI::Mouse* mouse );
	virtual JUI::IOResult on_mouse_clicked( JUI::Mouse* mouse );
	virtual JUI::IOResult on_mouse_released( JUI::Mouse* mouse );

	// Keyboard handling.
	virtual JUI::IOResult on_key_pressed( int key_code );
	virtual JUI::IOResult on_key_released( int key_code );

	// Position alignment.
	void center_to( const Container* parent );

protected:

    // UI layout variables.
    JUI::VerticalLayout* layout_;

private:

    // Popup active state.
	PopupState state_;

};

#endif // POPUP_H