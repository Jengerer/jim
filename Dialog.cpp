#include "Dialog.h"

Texture* Dialog::texture = NULL;

Dialog::Dialog( const string& message )
{
	setMessage( message);
}

void Dialog::draw( DirectX* directX )
{
	// Draw the background texture.
	directX->drawTexture( texture, x, y );

	// Center the text.
	RECT rect;
	rect.left	= (long)x + PADDING;
	rect.top	= (long)y + PADDING;
	rect.right	= (long)x + getWidth()	- PADDING;
	rect.bottom	= (long)y + getHeight() - PADDING;

	// Draw it.
	directX->drawText(message_, &rect, DT_CENTER, D3DCOLOR_ARGB( 255, 255, 255, 255 ));
}

void Dialog::onMouseEvent( MouseListener* mouseListener, EMouseEvent mEvent )
{
	// Nothing for dialogs yet.
}

void Dialog::setMessage( const string& message )
{
	message_ = message;
}

void Dialog::appendMessage( const string& message )
{
	message_ += message;
}

const Texture* Dialog::getTexture() const
{
	return texture;
}