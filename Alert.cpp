#include "Alert.h"

Alert::Alert( const string& message ) : Dialog( message )
{
	// Make OK button.
	okButton = new Button( "okay" );
	resize();
}

Alert::~Alert()
{
	if (okButton != 0)
	{
		delete okButton;
		okButton = 0;
	}
}

void Alert::draw( DirectX *directX )
{
	// Draw like our parent.
	Dialog::draw( directX );
	okButton->draw( directX );
}

void Alert::updatePosition()
{
	// Move the button.
	float buttonX = getX() + getWidth()/2 - okButton->getWidth()/2;
	float buttonY = getY() + getHeight() - okButton->getHeight() - DIALOG_PADDING - DIALOG_STROKE_WIDTH;
	okButton->setPosition( buttonX, buttonY );
}

void Alert::resize()
{
	Dialog::resize();
	setSize( getWidth(), getHeight() + okButton->getHeight() + DIALOG_PADDING );
	updatePosition();
}

void Alert::setMessage( const string& message )
{
	Dialog::setMessage( message );
	resize();
}

const Button* Alert::getButton() const
{
	return okButton;
}