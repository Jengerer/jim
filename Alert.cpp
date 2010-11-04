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
	okButton->draw( directX );
}

void Alert::setPosition( float x, float y )
{
	Component::setPosition( x, y );
	updatePosition();
}

void Alert::updatePosition()
{
	// Move the button.
	float buttonX = getX() + getWidth()/2 - okButton->getWidth()/2;
	float buttonY = getY() + getHeight() - okButton->getHeight() - ALERT_PADDING;
	okButton->setPosition( buttonX, buttonY );
}

const Button* Alert::getButton() const
{
	return okButton;
}