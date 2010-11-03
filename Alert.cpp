#include "Alert.h"

const int	ALERT_PADDING	= 20;

Alert::Alert( const string& message ) : Dialog( message )
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
	Dialog::draw( directX );

	// Just add the button.
	okButton->x = getX() + getWidth()/2 - okButton->getWidth()/2;
	okButton->y = getY() + getHeight() - okButton->getHeight() - ALERT_PADDING;
	okButton->draw( directX );
}

const Button* Alert::getButton() const
{
	return okButton;
}