#include "popup_display.hpp"
#include <algorithm>

/*
 * Popup layer constructor.
 */
PopupDisplay::PopupDisplay( void )
{
	// No default handler.
	set_popup_handler( nullptr );
}

/*
 * Popup layer constructor by position.
 */
PopupDisplay::PopupDisplay( int x, int y ) : Container( x, y )
{
	// No default handler.
	set_popup_handler( nullptr );
}

/*
 * Sets the interface to call when popup events occur.
 */
void PopupDisplay::set_popup_handler( PopupHandler* handler )
{
	handler_ = handler;
}

/*
 * Creates a notice and places it on top of the popup layer.
 * Returns pointer to notice if succeeded, nullptr otherwise.
 */
Notice* PopupDisplay::create_notice( const JUTIL::String* message )
{
	// Create new notice.
    Notice* notice;
    if (!JUTIL::BaseAllocator::allocate( &notice )) {
        return nullptr;
    }
    notice = new (notice) Notice();

    // Initialize notice layout and message.
    if (!notice->initialize( message ) || !add_popup( notice )) {
        JUTIL::BaseAllocator::destroy( notice );
        return nullptr;
    }
	return notice;
}

/*
 * Creates an alert and places it on top of the popup layer.
 * Returns pointer to alert if succeeded, nullptr otherwise.
 */
Alert* PopupDisplay::create_alert( const JUTIL::String* message )
{
	// Create new alert.
    Alert* alert;
    if (!JUTIL::BaseAllocator::allocate( &alert )) {
        return nullptr;
    }
    alert = new (alert) Alert();

    // Initialize notice layout and message.
    if (!alert->initialize( message ) || !add_popup( alert )) {
        JUTIL::BaseAllocator::destroy( alert );
        return nullptr;
    }
	return alert;
}

/*
 * Creates a confirmation and places it on top of the popup layer.
 * Returns pointer to confirmation if succeeded, nullptr otherwise.
 */
Confirmation* PopupDisplay::create_confirmation( const JUTIL::String* question )
{
	// Create new alert.
    Confirmation* confirmation;
    if (!JUTIL::BaseAllocator::allocate( &confirmation )) {
        return nullptr;
    }
    confirmation = new (confirmation) Confirmation();

    // Initialize notice layout and message.
    if (!confirmation->initialize( question ) || !add_popup( confirmation )) {
        JUTIL::BaseAllocator::destroy( confirmation );
        return nullptr;
    }
	return confirmation;
}

bool PopupDisplay::add_popup( Popup* popup )
{
    // Try adding.
    if (!add( popup )) {
        return false;
    }
    // TODO: Make sure to remove popup if pushing to list fails.
    popups_.push_front( popup );

	// Keep in center.
	popup->center_to( this );
    return true;
}

void PopupDisplay::hide_popup( Popup* popup )
{
	// Remove from list.
    popups_.remove( popup );
	remove( popup );
}

void PopupDisplay::delete_popup( Popup* popup )
{
	// Full destructive remove.
	hide_popup( popup );
	JUTIL::BaseAllocator::destroy( popup );
};

/*
 * Pass mouse click event to top popup if one exists.
 */
JUI::IOResult PopupDisplay::on_mouse_clicked( JUI::Mouse* mouse )
{
	// Check list front.
	if (has_popup()) {
		Popup* top = get_top_popup();

        // Pass message to popup.
        JUI::IOResult result = top->on_mouse_clicked( mouse );
        if (result == JUI::IO_RESULT_HANDLED) {
            if (!handle_popup_state( top )) {
                return JUI::IO_RESULT_ERROR;
            }
        }

        return result;
	}

    return JUI::IO_RESULT_UNHANDLED;
}

/*
 * Pass mouse release event to top popup if one exists.
 */
JUI::IOResult PopupDisplay::on_mouse_clicked( JUI::Mouse* mouse )
{
	// Check list front.
	if (has_popup()) {
		Popup* top = get_top_popup();

        // Pass message to popup.
        JUI::IOResult result = top->on_mouse_released( mouse );
        if (result == JUI::IO_RESULT_HANDLED) {
            if (!handle_popup_state( top )) {
                return JUI::IO_RESULT_ERROR;
            }
        }

        return result;
	}

    return JUI::IO_RESULT_UNHANDLED;
}

/*
 * Pass mouse movement event to top popup if one exists.
 */
JUI::IOResult PopupDisplay::on_mouse_moved( JUI::Mouse* mouse )
{
	// Check list front.
	if (has_popup()) {
		Popup* top = get_top_popup();
		JUI::IOResult result = top->on_mouse_moved( mouse );
        return result;
	}

	// Always let other components get movement.
    return JUI::IO_RESULT_UNHANDLED;
}

/*
 * Pass key pressed event to top popup if one exists.
 */
JUI::IOResult PopupDisplay::on_key_pressed( int key )
{
	// Check list front.
	if (has_popup()) {
		Popup* top = get_top_popup();
        JUI::IOResult result = top->on_key_pressed( key );
        if (result == JUI::IO_RESULT_HANDLED) {
            if (!handle_popup_state( top )) {
                return JUI::IO_RESULT_ERROR;
            }
		}

		return result;
	}

    return JUI::IO_RESULT_UNHANDLED;
}

/*
 * Pass key released event to top popup if one exists.
 */
JUI::IOResult PopupDisplay::on_key_pressed( int key )
{
	// Check list front.
	if (has_popup()) {
		Popup* top = get_top_popup();
        JUI::IOResult result = top->on_key_pressed( key );
        if (result == JUI::IO_RESULT_HANDLED) {
            if (!handle_popup_state( top )) {
                return JUI::IO_RESULT_ERROR;
            }
		}

		return result;
	}

    return JUI::IO_RESULT_UNHANDLED;
}

/*
 * Returns a pointer to the most relevant popup.
 */
Popup* PopupDisplay::get_top_popup( void ) const
{
	return popups_.front();
}

/*
 * Returns whether a popup exists in this display.
 */
bool PopupDisplay::has_popup( void ) const
{
	return !popups_.empty();
}

/*
 * Handle a possible change in popup state.
 * Returns true if handling successful, false otherwise.
 */
bool PopupDisplay::handle_popup_state( Popup* popup )
{
	// Check if we should remove popup.
	switch (popup->get_state()) {
	case POPUP_STATE_KILLED:
        if (!handler_->on_popup_killed( popup )) {
            return false;
        }
		delete_popup( popup );
		break;
	case POPUP_STATE_HIDDEN:
		hide_popup( popup );
		break;
	}

    // Nothing bad happened, return success.
    return true;
}
