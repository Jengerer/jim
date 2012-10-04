#ifndef POPUP_DISPLAY_H
#define POPUP_DISPLAY_H

#include <list>
#include <string/constant_string.hpp>

#include <jui/layout/container.hpp>
#include <jui/io/keyboard_handler_interface.hpp>
#include <jui/io/mouse_handler_interface.hpp>

#include "alert.hpp"
#include "confirmation.hpp"
#include "ipopup_handler.hpp"
#include "notice.hpp"

class PopupDisplay: public JUI::Container, public JUI::MouseHandlerInterface, public JUI::KeyboardHandlerInterface
{
	
public:

	PopupDisplay( float x = 0.0f, float y = 0.0f );

	// Set-up.
	void			set_popup_handler( IPopupHandler* handler );

	// Popup creators.
	Notice*			create_notice( const JUTIL::ConstantString& message );
	Alert*			create_alert( const JUTIL::ConstantString& message );
	Confirmation*	create_confirmation( const JUTIL::ConstantString& question );

	// Popup handling.
	bool	has_popup( void ) const;
	bool    add_popup( Popup* popup );
	void	hide_popup( Popup* popup );
	void	remove_popup( Popup* popup );
	
	// JUI::Mouse* handling for popups.
	virtual bool	on_mouse_clicked( JUI::Mouse* mouse );
	virtual bool	on_mouse_released( JUI::Mouse* mouse );
	virtual bool	on_mouse_moved( JUI::Mouse* mouse );

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