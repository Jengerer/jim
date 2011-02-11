#include "Alert.h"

Alert::Alert( const string& message ) : Dialog( message )
{
	// Make OK button.
	okButton = new Button( "okay" );
	add( okButton );

	resize();
}

Alert::~Alert()
{
	// Button removed as component.
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

bool Alert::mouseMoved( Mouse *mouse )
{
	// Parent behaviour.
	Dialog::mouseMoved( mouse );

	// Pass state to button.
	okButton->mouseMoved( mouse );
	return true;
}

bool Alert::mouseClicked( Mouse *mouse )
{
	if (!mouse->isTouching( okButton )) {
		Draggable::mouseClicked( mouse );
	}

	return true;
}

bool Alert::mouseReleased( Mouse *mouse )
{
	// Draggable behaviour.
	Draggable::mouseReleased( mouse );

	// Now set state if we're terminated.
	if (mouse->isTouching( okButton )) {
		setState( POPUP_STATE_INACTIVE );
	}

	return true;
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