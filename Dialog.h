#pragma once

#include "Drawable.h"
#include "Popup.h"

enum EPopupType {
	POPUP_TYPE_DIALOG,
	POPUP_TYPE_ALERT,
	POPUP_TYPE_CONFIRM
};

class Dialog: public Popup
{
public:
	Dialog( const string& message );
	virtual ~Dialog();

	// Drawable functions.
	void			draw( DirectX* directX );
	virtual void	resize();

	// Message handling.
	const string* getMessage() const { return &message_; }
	void setMessage( const string& message );
	void appendMessage( const string& message );

	// Class-wide texture.
	static Texture* texture;

private:
	string message_;
};