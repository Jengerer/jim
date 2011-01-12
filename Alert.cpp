#include "Alert.h"

const int	ALERT_PADDING	= 20;
const int	ALERT_WIDTH		= 250;

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

void Alert::draw( DirectX* directX )
{
	// Draw like our parent.
	Dialog::draw( directX );
	okButton->draw( directX );
}

void Alert::updatePosition()
{
	// Move the button.
	float buttonX = getX() + getWidth()/2 - okButton->getWidth()/2;
	float buttonY = getY() + getHeight() - okButton->getHeight() - ALERT_PADDING;
	okButton->setPosition( buttonX, buttonY );
}

void Alert::resize()
{
	// Get screen device context.
	HDC hdc = GetDC( NULL );

	// Calculate size of text.
	RECT size = {0, 0, ALERT_WIDTH, 0};
	DrawText( hdc, getMessage()->c_str(), -1, &size, DT_CALCRECT | DT_WORDBREAK );

	// Store old, and set new.
	int oldWidth = getWidth();
	int oldHeight = getHeight();
	setSize( size.right - size.left + ALERT_PADDING*2, size.bottom - size.top + okButton->getHeight() + ALERT_PADDING*3);

	// Keep alignment.
	float diffWidth = getWidth() - oldWidth;
	float diffHeight = getHeight() - oldHeight;
	float newX = getX() - diffWidth/2;
	float newY = getY() - diffHeight/2;
	setPosition( (int)newX, (int)newY );
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