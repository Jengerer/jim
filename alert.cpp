#include "alert.hpp"

Alert::Alert( const std::string& message ) : Notice( message )
{
	// Make OK button.
	ok_ = Button::create_label_button( "okay" );
	content_->add( ok_ );
	pack();
}

Alert::~Alert()
{
	// Button removed as component.
}

const Button* Alert::GetButton( void ) const
{
	return ok_;
}

bool Alert::on_key_released( int key )
{
	if (key == VK_RETURN) {
		set_state( POPUP_STATE_KILLED );
	}

	return true;
}

bool Alert::on_mouse_moved( JUI::Mouse* mouse )
{
	// Parent behaviour.
	ok_->on_mouse_moved( mouse );
	return Notice::on_mouse_moved( mouse );
}

bool Alert::on_mouse_clicked( JUI::Mouse* mouse )
{
	return Notice::on_mouse_clicked( mouse );
}

bool Alert::on_mouse_released( JUI::Mouse* mouse )
{
	if (mouse->is_touching( this )) {
		if (ok_->on_mouse_released( mouse )) {
			set_state( POPUP_STATE_KILLED );
		}

		return true;
	}

	return false;
}