#pragma once

#include "Drawable.h"
#include "Popup.h"

#define PADDING 20

class Dialog: public Popup
{
public:
	Dialog( const string& message );

	// Drawable functions.
	void draw( DirectX* directX );
	virtual void onMouseEvent( MouseListener* mouseListener, EMouseEvent mEvent );

	// Message handling.
	void setMessage( const string& message );
	void appendMessage( const string& message );

	// Class-wide texture.
	virtual const Texture* getTexture() const;
	static Texture* texture;

private:
	string message_;
};