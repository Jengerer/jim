#pragma once

#include "container.h"
#include "draggable.h"
#include "imouse_handler.h"
#include "ikeyboard_handler.h"

enum EPopupState
{
	POPUP_STATE_ACTIVE,
	POPUP_STATE_HIDDEN,
	POPUP_STATE_KILLED,
};

class Popup: public ConstrainedContainer, public IMouseHandler, public IKeyboardHandler
{
public:

	Popup( float x = 0.0f, float y = 0.0f );
	virtual ~Popup();

	// Get activity state.
	void			SetState( EPopupState state );
	EPopupState		GetState( void ) const;

	// Mouse handling.
	virtual bool	on_mouse_clicked( Mouse* mouse );
	virtual bool	on_mouse_released( Mouse* mouse );
	virtual bool	on_mouse_moved( Mouse* mouse );

	// Keyboard handling.
	virtual bool	on_key_pressed( Key key );
	virtual bool	on_key_released( Key key );

	// Position alignment.
	void			CenterTo( const Container* parent );

private:
	EPopupState		state_;
};