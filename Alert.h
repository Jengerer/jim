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
	virtual void updatePosition();
	virtual void resize();
	virtual void setMessage( const string& message );
	const Button* getButton() const;

	// Mouse handling.
	virtual bool mouseMoved( Mouse *mouse );
	virtual bool mouseClicked( Mouse *mouse );
	virtual bool mouseReleased( Mouse *mouse );

private:
	Button* okButton;
};