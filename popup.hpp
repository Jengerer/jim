#ifndef POPUP_H
#define POPUP_H

#include <jui/layout/container.hpp>
#include <jui/io/mouse_handler_interface.hpp>
#include <jui/io/keyboard_handler_interface.hpp>

#include "draggable.hpp"

enum EPopupState
{
	POPUP_STATE_ACTIVE,
	POPUP_STATE_HIDDEN,
	POPUP_STATE_KILLED,
};

class Popup: public JUI::ConstrainedContainer, public JUI::MouseHandlerInterface, public JUI::KeyboardHandlerInterface
{
public:

	Popup( float x = 0.0f, float y = 0.0f );
	virtual ~Popup();

	// Get activity state.
	void			set_state( EPopupState state );
	EPopupState		get_state( void ) const;
	bool			is_active( void ) const;
	bool			is_hidden( void ) const;
	bool			is_killed( void ) const;

	// JUI::Mouse* handling.
	virtual bool	on_mouse_clicked( JUI::Mouse* mouse );
	virtual bool	on_mouse_released( JUI::Mouse* mouse );
	virtual bool	on_mouse_moved( JUI::Mouse* mouse );

	// Keyboard handling.
	virtual bool	on_key_pressed( int key_code );
	virtual bool	on_key_released( int key_code );

	// Position alignment.
	void			center_to( const Container* parent );

private:
	EPopupState		state_;
};

#endif // POPUP_H