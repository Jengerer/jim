#include "Dialog.h"

Texture*	Dialog::texture	= 0;
const int	DIALOG_PADDING	= 20;

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
	rect.left	= (long)x + DIALOG_PADDING;
	rect.top	= (long)y + DIALOG_PADDING;
	rect.right	= (long)x + getWidth()		- DIALOG_PADDING;
	rect.bottom	= (long)y + getHeight()		- DIALOG_PADDING;

	// Draw it.
	directX->drawText(message_, &rect, DT_CENTER | DT_WORDBREAK, D3DCOLOR_ARGB( 255, 255, 255, 255 ));
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

int Dialog::getWidth() const
{
	return texture->getWidth();
}

int Dialog::getHeight() const
{
	return texture->getHeight();
}