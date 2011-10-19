#include "alert.h"

Alert::Alert( const std::string& message ) : Notice( message )
{
	// Make OK button.
	okButton_ = Button::CreateLabelButton( "okay" );
	content_->Add( okButton_ );
	Pack();
}

Alert::~Alert()
{
	// Button removed as component.
}

void Alert::Pack()
{
	okButton_->Pack();
	Notice::Pack();
}

void Alert::SetMessage( const std::string& message )
{
	Notice::SetMessage( message );
}

const Button* Alert::GetButton( void ) const
{
	return okButton_;
}

bool Alert::MouseMoved( Mouse *mouse )
{
	// Parent behaviour.
	okButton_->MouseMoved( mouse );
	return Notice::MouseMoved( mouse );
}

bool Alert::MouseClicked( Mouse *mouse )
{
	if (mouse->IsTouching( this )) {
		return true;
	}

	return false;
}

bool Alert::MouseReleased( Mouse *mouse )
{
	if (mouse->IsTouching( this )) {
		if (okButton_->MouseReleased( mouse )) {
			SetState( POPUP_STATE_KILLED );
		}

		return true;
	}

	return false;
}