#include "Alert.h"

Alert::Alert( const string& message ) : Notice( message )
{
	// Make OK button.
	okButton_ = new LabelButton( "okay", nullptr );
	okButton_->Pack();
	Add( okButton_ );
	Pack();
}

Alert::~Alert( void)
{
	// Button removed as component.
}

void Alert::OnDraw( DirectX *directX )
{
	// Draw like our parent.
	Notice::OnDraw( directX );
	okButton_->OnDraw( directX );
}

void Alert::Pack()
{
	Notice::Pack();
	SetSize( GetWidth(), GetHeight() + okButton_->GetHeight() + NOTICE_PADDING );
	
	// Center okay button under text.
	okButton_->SetLocalPosition( 
		(GetWidth() - okButton_->GetWidth()) / 2.0f,
		(GetHeight() - okButton_->GetHeight()) / 2.0f - NOTICE_PADDING - NOTICE_STROKE_WIDTH );

	UpdatePosition();
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