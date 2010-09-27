#include "Alert.h"

const int	ALERT_PADDING	= 20;

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
	Dialog::draw(directX);

	// Just add the button.
	okButton->x = x + getWidth()/2 - okButton->getWidth()/2;
	okButton->y = y + getHeight() - okButton->getHeight() - ALERT_PADDING;
	okButton->draw( directX );
}

void Alert::onMouseEvent( MouseListener* mouseListener, EMouseEvent mEvent )
{
	switch (mEvent) {
	case MOUSE_EVENT_RELEASE:
		if (okButton->mouseTouching( mouseListener ))
			buttonListener_->onButtonRelease( okButton );
		break;
	default:
		// Pass the event to the button.
		okButton->onMouseEvent( mouseListener, mEvent );
		break;
	}
}

void Alert::setButtonListener( ButtonListener* buttonListener)
{
	buttonListener_ = buttonListener;
}

const Button* Alert::getButton() const
{
	return okButton;
}