#include "alert.h"

Alert::Alert( const string& message ) : Notice( message )
{
	// Make OK button.
	okButton_ = new LabelButton( "okay", nullptr );
	content_->Add( okButton_ );
	Pack();
}

Alert::~Alert( void)
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
	Notice::OnMouseMoved( mouse );
	okButton_->OnMouseMoved( mouse );
	return true;
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