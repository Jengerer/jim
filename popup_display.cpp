#include "popup_display.hpp"
#include <algorithm>

/*
 * Popup layer constructor.
 */
PopupDisplay::PopupDisplay( float x, float y ) : Container( x, y )
{
	// No default handler.
	set_popup_handler( nullptr );
}

/*
 * Sets the interface to call when popup events occur.
 */
void PopupDisplay::set_popup_handler( IPopupHandler* handler )
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
	auto i = find( popups_.begin(), popups_.end(), popup );
	if (i == popups_.end()) {
		throw std::runtime_error( "Tried to remove popup that didn't exist." );
	}
	popups_.erase( i );

	// Remove from container.
	remove( popup );
}

void PopupDisplay::remove_popup( Popup* popup )
{
	// Full removal.
	hide_popup( popup );
	JUTIL::BaseAllocator::destroy( popup );
};

JUI::IOResult PopupDisplay::on_mouse_clicked( JUI::Mouse* mouse )
{
	// Check list front.
	if (has_popup()) {
		Popup* top = get_top_popup();

		// Send the popup the message.
		if (top->on_mouse_clicked( mouse )) {
			// Pass to handler.
			if (handler_ != nullptr) {
				handler_->on_popup_clicked( top );
			}

			// Remove if killed/hidden.
			handle_popup_state( top );
		}

        return JUI::IO_RESULT_HANDLED;
	}

    return JUI::IO_RESULT_UNHANDLED;
}

JUI::IOResult PopupDisplay::on_mouse_released( JUI::Mouse* mouse )
{
	// Check list front.
	if (has_popup()) {
		Popup* top = get_top_popup();

		// Send the popup the message.
		JUI::IOResult result = top->on_mouse_released( mouse );
		if (result == JUI::IO_RESULT_HANDLED) {
			// Pass to handler.
			if (handler_ != nullptr) {
				if (!handler_->on_popup_released( top )) {
					return JUI::IO_RESULT_ERROR;
				}
			}

			// Check if we should remove popup.
			handle_popup_state( top );
		}

		return JUI::IO_RESULT_HANDLED;
	}

    return JUI::IO_RESULT_UNHANDLED;
}

JUI::IOResult PopupDisplay::on_mouse_moved( JUI::Mouse* mouse )
{
	// Check list front.
	if (has_popup()) {
		Popup* top = get_top_popup();
		return top->on_mouse_moved( mouse );
	}

	// Always let other components get movement.
    return JUI::IO_RESULT_UNHANDLED;
}

JUI::IOResult PopupDisplay::on_key_pressed( int key )
{
	// Check list front.
	if (has_popup()) {
		Popup* top = get_top_popup();
		
		// Send popup the message.
		if (top->on_key_pressed( key )) {
			// Pass to handler.
			if (handler_ != nullptr) {
				handler_->on_popup_key_pressed( top );
			}

			// Check if we should remove popup.
			handle_popup_state( top );
		}

		return JUI::IO_RESULT_HANDLED;
	}

    return JUI::IO_RESULT_UNHANDLED;
}

JUI::IOResult PopupDisplay::on_key_released( int key )
{
	// Check list front.
	if (has_popup()) {
		Popup* top = get_top_popup();

		// Send popup the message.
		if (top->on_key_released( key )) {
			// Pass to handler.
			if (handler_ != nullptr) {
				handler_->on_popup_key_released( top );
			}

			// Check if we should remove popup.
			handle_popup_state( top );
		}

		return JUI::IO_RESULT_HANDLED;
	}

    return JUI::IO_RESULT_UNHANDLED;
}

Popup* PopupDisplay::get_top_popup( void ) const
{
	return popups_.front();
}

bool PopupDisplay::has_popup( void ) const
{
	return !popups_.empty();
}

void PopupDisplay::handle_popup_state( Popup* popup )
{
	// Check if we should remove popup.
	switch (popup->get_state()) {
	case POPUP_STATE_KILLED: // Intentional overflow.
		remove_popup( popup );
		break;
	case POPUP_STATE_HIDDEN:
		hide_popup( popup );
		break;
	}
}