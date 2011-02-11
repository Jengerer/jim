#pragma once

#include "Drawable.h"
#include "Popup.h"

#define DIALOG_WIDTH			300
#define DIALOG_RADIUS			10
#define DIALOG_PADDING			20
#define DIALOG_STROKE_WIDTH		5

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
	static Font *font;

private:
	string message_;
};