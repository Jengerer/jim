#include "alert.h"

Alert::Alert( const string& message ) : Notice( message )
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

void Alert::SetMessage( const string& message )
{
	Notice::SetMessage( message );
}

const Button* Alert::GetButton( void ) const
{
	return okButton_;
}

bool Alert::OnMouseMoved( Mouse *mouse )
{
	// Parent behaviour.
	okButton_->OnMouseMoved( mouse );
	return Notice::OnMouseMoved( mouse );
}

bool Alert::OnLeftClicked( Mouse *mouse )
{
	if (mouse->IsTouching( this )) {
		if (!mouse->IsTouching( okButton_ )) {
			Draggable::OnLeftClicked( mouse );
		}

		return true;
	}

	return false;
}

bool Alert::OnLeftReleased( Mouse *mouse )
{
	if (mouse->IsTouching( this )) {
		// Now set state if we're terminated.
		if (mouse->IsTouching( okButton_ )) {
			SetState( POPUP_STATE_KILLED );
		}
		else {
			Draggable::OnLeftReleased( mouse );
		}

		return true;
	}

	return false;
}