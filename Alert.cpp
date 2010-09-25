#include "Alert.h"

Alert::Alert( const string& message ): Dialog( message )
{
	// Make OK button.
	okButton = new Button( "okay" );
}

Alert::~Alert()
{
	if (okButton != 0)
	{
		delete okButton;
		okButton = 0;
	}
}

void Alert::draw( DirectX* directX )
{
	// Draw like our parent.
	Dialog::drawObject(directX);

	// Just add the button.
	okButton->x = x + getWidth()/2 - okButton->getWidth()/2;
	okButton->y = y + getHeight()/2 - okButton->getHeight()/2 - PADDING;
	okButton->draw( directX );
}

void Alert::onMouseEvent( MouseListener* mouseListener, EMouseEvent mEvent )
{
	switch ( mEvent )
	{
	case MOUSE_EVENT_RELEASE:
		if ( okButton->mouseTouching( mouseListener ) )
		{
			state = POPUP_STATE_INACTIVE;
		}
		break;
	default:
		// Pass the event to the button.
		okButton->onMouseEvent( mouseListener, mEvent );
	}
}