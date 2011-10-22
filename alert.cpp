#include "alert.h"

Alert::Alert( const std::string& message ) : Notice( message )
{
	// Make OK button.
	ok_ = Button::CreateLabelButton( "okay" );
	content_->add( ok_ );
	Pack();
}

Alert::~Alert()
{
	// Button removed as component.
}

const Button* Alert::GetButton( void ) const
{
	return ok_;
}

bool Alert::on_key_released( Key key )
{
	if (key == VK_RETURN) {
		SetState( POPUP_STATE_KILLED );
	}

	return true;
}

bool Alert::on_mouse_moved( Mouse *mouse )
{
	// Parent behaviour.
	ok_->on_mouse_moved( mouse );
	return Notice::on_mouse_moved( mouse );
}

bool Alert::on_mouse_clicked( Mouse *mouse )
{
	return Notice::on_mouse_clicked( mouse );
}

bool Alert::on_mouse_released( Mouse *mouse )
{
	if (mouse->IsTouching( this )) {
		if (ok_->on_mouse_released( mouse )) {
			SetState( POPUP_STATE_KILLED );
		}

		return true;
	}

	return false;
}