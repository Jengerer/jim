#ifndef POPUP_DISPLAY_H
#define POPUP_DISPLAY_H

#include <list>
#include <string>

#include <jui/ikeyboard_handler.h>
#include <jui/imouse_handler.h>

#include "alert.h"
#include "confirmation.h"
#include "ipopup_handler.h"
#include "notice.h"

class PopupDisplay: public Container, public IMouseHandler, public IKeyboardHandler
{
	
public:

	PopupDisplay( float x = 0.0f, float y = 0.0f );

	// Set-up.
	void			set_popup_handler( IPopupHandler* handler );

	// Popup creators.
	Notice*			create_notice( const std::string& message );
	Alert*			create_alert( const std::string& message );
	Confirmation*	create_confirmation( const std::string& question );

	// Popup handling.
	bool	has_popup( void ) const;
	void	add_popup( Popup* popup );
	void	hide_popup( Popup* popup );
	void	remove_popup( Popup* popup );
	
	// Mouse handling for popups.
	virtual bool	on_mouse_clicked( Mouse* mouse );
	virtual bool	on_mouse_released( Mouse* mouse );
	virtual bool	on_mouse_moved( Mouse* mouse );

	// Keyboard handling for popups.
	virtual bool	on_key_pressed( int key );
	virtual bool	on_key_released( int key );

private:

	Popup*	get_top_popup( void ) const;
	void	handle_popup_state( Popup* popup );

private:

	
	std::list<Popup*> popups_;
	IPopupHandler* handler_;

};

#endif // POPUP_DISPLAY_H