#pragma once

#include "Container.h"
#include "Dialog.h"
#include "Button.h"

class Alert: public Dialog
{
public:
	Alert( const string& message );
	virtual ~Alert();

	void draw( DirectX* directX );
	virtual void setPosition( float x, float y );
	virtual void updatePosition();
	const Button* getButton() const;

private:
	Button* okButton;
};