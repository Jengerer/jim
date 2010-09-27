#pragma once

#include "Dialog.h"
#include "Button.h"

class Alert: public Dialog
{
public:
	Alert( const string& message );
	~Alert();

	void draw( DirectX* directX );
	void onMouseEvent( MouseListener* mouseListener, EMouseEvent mEvent );

	// Button listener methods.
	void setButtonListener( ButtonListener* buttonListener );

	const Button* getButton() const;

private:
	Button* okButton;
	ButtonListener* buttonListener_;
};