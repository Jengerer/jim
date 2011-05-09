#include "Alert.h"

Alert::Alert( const string& message ) : Notification( message )
{
	// Make OK button.
	okButton = new Button( "okay" );
	okButton->Pack();
	Add( okButton );
	Pack();
}

Alert::~Alert( void)
{
	// Button removed as component.
}

void Alert::OnDraw( DirectX *directX )
{
	// Draw like our parent.
	Notification::OnDraw( directX );
	okButton->OnDraw( directX );
}

void Alert::UpdatePosition( void )
{
	// Move the button.
	float buttonX = GetX() + GetWidth()/2 - okButton->GetWidth()/2;
	float buttonY = GetY() + GetHeight() - okButton->GetHeight() - NOTIFICATION_PADDING - NOTIFICATION_STROKE_WIDTH;
	okButton->SetPosition( buttonX, buttonY );
}

void Alert::Pack()
{
	Notification::Pack();
	SetSize( GetWidth(), GetHeight() + okButton->GetHeight() + NOTIFICATION_PADDING );
	UpdatePosition();
}

void Alert::SetMessage( const string& message )
{
	Notification::SetMessage( message );
}

const Button* Alert::GetButton( void ) const
{
	return okButton;
}

bool Alert::OnMouseMoved( Mouse *mouse )
{
	// Parent behaviour.
	Notification::OnMouseMoved( mouse );
	okButton->OnMouseMoved( mouse );
	return true;
}

bool Alert::OnLeftClicked( Mouse *mouse )
{
	if (mouse->isTouching( this )) {
		if (!mouse->isTouching( okButton )) {
			Draggable::OnLeftClicked( mouse );
		}

		return true;
	}

	return false;
}

bool Alert::OnLeftReleased( Mouse *mouse )
{
	if (mouse->isTouching( this )) {
		// Now set state if we're terminated.
		if (mouse->isTouching( okButton )) {
			setState( POPUP_STATE_KILLED );
		}
		else {
			Draggable::OnLeftReleased( mouse );
		}

		return true;
	}

	return false;
}

bool Alert::OnRightClicked( Mouse *mouse )
{
	return true;
}

bool Alert::OnRightReleased( Mouse *mouse )
{
	return true;
}