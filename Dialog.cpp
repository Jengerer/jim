#include "Dialog.h"

Texture*	Dialog::texture	= 0;
const int	DIALOG_PADDING	= 20;

Dialog::Dialog( const string& message )
{
	setMessage( message );
}

Dialog::~Dialog()
{
	// Dialog is destroyed.
}

void Dialog::draw( DirectX* directX )
{
	// Draw the background texture.
	directX->drawTexture( texture, getX(), getY() );

	// Center the text.
	RECT rect;
	rect.left	= (long)getX() + DIALOG_PADDING;
	rect.top	= (long)getY() + DIALOG_PADDING;
	rect.right	= (long)getX() + getWidth()		- DIALOG_PADDING;
	rect.bottom	= (long)getY() + getHeight()		- DIALOG_PADDING;

	// Draw it.
	directX->drawText(message_, &rect, DT_CENTER | DT_WORDBREAK, D3DCOLOR_ARGB( 255, 255, 255, 255 ));
}

void Dialog::setMessage( const string& message )
{
	message_ = message.c_str();
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