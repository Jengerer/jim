#ifndef POPUP_DISPLAY_HPP
#define POPUP_DISPLAY_HPP

#include <list>
#include <string/constant_string.hpp>
#include <jui/layout/container.hpp>
#include <jui/io/keyboard_handler_interface.hpp>
#include <jui/io/mouse_handler_interface.hpp>
#include "alert.hpp"
#include "confirmation.hpp"
#include "popup_listener.hpp"
#include "notice.hpp"

class PopupDisplay: public JUI::Container, public JUI::MouseHandlerInterface, public JUI::KeyboardHandlerInterface
{
	
public:

    PopupDisplay( void );
	PopupDisplay( int x, int y );

	// Set-up.
	void set_popup_listener( PopupListener* handler );

	// Popup creators.
	Notice* create_notice( const JUTIL::String* message );
	Alert* create_alert( const JUTIL::String* message );
	Confirmation* create_confirmation( const JUTIL::String* question );

	// Popup handling.
	bool has_popup( void ) const;
	bool add_popup( Popup* popup );
	void hide_popup( Popup* popup );
	void delete_popup( Popup* popup );
	
	// Mouse handling for popups.
	virtual JUI::IOResult on_mouse_clicked( JUI::Mouse* mouse );
	virtual JUI::IOResult on_mouse_released( JUI::Mouse* mouse );
	virtual JUI::IOResult on_mouse_moved( JUI::Mouse* mouse );

	// Keyboard handling for popups.
	virtual JUI::IOResult on_key_pressed( int key );
	virtual JUI::IOResult on_key_released( int key );

private:

    // Popup stack handling.
	Popup* get_top_popup( void ) const;
	bool handle_popup_state( Popup* popup );

private:
	
	std::list<Popup*> popups_;
	PopupListener* handler_;

};

#endif // POPUP_DISPLAY_HPP