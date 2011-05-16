#include "Alert.h"

Alert::Alert( const string& message ) : Notice( message )
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
	Notice::OnDraw( directX );
	okButton->OnDraw( directX );
}

void Alert::UpdatePosition( void )
{
	// Move the button.
	float buttonX = GetX() + GetWidth()/2 - okButton->GetWidth()/2;
	float buttonY = GetY() + GetHeight() - okButton->GetHeight() - NOTICE_PADDING - NOTICE_STROKE_WIDTH;
	okButton->SetPosition( buttonX, buttonY );
}

void Alert::Pack()
{
	Notice::Pack();
	SetSize( GetWidth(), GetHeight() + okButton->GetHeight() + NOTICE_PADDING );
	UpdatePosition();
}

void Alert::SetMessage( const string& message )
{
	Notice::SetMessage( message );
}

const Button* Alert::GetButton( void ) const
{
	return okButton;
}

bool Alert::OnMouseMoved( Mouse *mouse )
{
	// Parent behaviour.
	Notice::OnMouseMoved( mouse );
	okButton->OnMouseMoved( mouse );
	return true;
}

bool Alert::OnLeftClicked( Mouse *mouse )
{
	if (mouse->IsTouching( this )) {
		if (!mouse->IsTouching( okButton )) {
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
		if (mouse->IsTouching( okButton )) {
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