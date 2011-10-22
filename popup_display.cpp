#include "popup_display.h"

#include <algorithm>

PopupDisplay::PopupDisplay( float x, float y ) : Container( x, y )
{
	// No default handler.
	set_popup_handler( nullptr );
}

void PopupDisplay::set_popup_handler( IPopupHandler* handler )
{
	handler_ = handler;
}

Notice* PopupDisplay::create_notice( const std::string& message )
{
	// Create new notice.
	Notice* notice = new Notice( message );
	add_popup( notice );

	return notice;
}

Alert* PopupDisplay::create_alert( const std::string& message )
{
	// Create new alert.
	Alert* alert = new Alert( message );
	add_popup( alert );

	return alert;
}

Confirmation* PopupDisplay::create_confirmation( const std::string& question )
{
	// Create new confirmation.
	Confirmation* confirmation = new Confirmation( question );
	add_popup( confirmation );

	return confirmation;
}

void PopupDisplay::add_popup( Popup* popup )
{
	add( popup );
	popups_.push_front( popup );
}

void PopupDisplay::remove_popup( Popup* popup )
{
	// Remove from list.
	auto i = find( popups_.begin(), popups_.end(), popup );
	if (i == popups_.end()) {
		throw std::runtime_error( "Tried to remove popup that didn't exist." );
	}
	popups_.erase( i );

	// Remove from container.
	remove( popup );
};

bool PopupDisplay::on_mouse_clicked( Mouse* mouse )
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

		return true;
	}

	return false;
}

bool PopupDisplay::on_mouse_released( Mouse* mouse )
{
	// Check list front.
	if (has_popup()) {
		Popup* top = get_top_popup();

		// Send the popup the message.
		if (top->on_mouse_released( mouse )) {
			// Pass to handler.
			if (handler_ != nullptr) {
				handler_->on_popup_released( top );
			}

			// Check if we should remove popup.
			handle_popup_state( top );
		}

		return true;
	}

	return false;
}

bool PopupDisplay::on_mouse_moved( Mouse* mouse )
{
	// Check list front.
	if (has_popup()) {
		Popup* top = get_top_popup();
		top->on_mouse_moved( mouse );
	}

	// Always let other components get movement.
	return false;
}

bool PopupDisplay::on_key_pressed( Key key )
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

		return true;
	}

	return false;
}

bool PopupDisplay::on_key_released( Key key )
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
		return true;
	}

	return false;
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
	switch (popup->GetState()) {
	case POPUP_STATE_KILLED: // Intentional overflow.
		delete popup;
	case POPUP_STATE_HIDDEN:
		remove_popup( popup );
		break;
	}
}